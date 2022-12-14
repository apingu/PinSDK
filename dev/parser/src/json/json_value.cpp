/*
 * File JSONValue.cpp part of the SimpleJSON Library - http://mjpa.in/json
 *
 * Copyright (C) 2010 Mike Anchor
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//#define _USE_MATH_DEFINES

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <math.h>

#include "json_value.h"

// Macros to free an array/object
#define FREE_ARRAY(x) { JSONArray::iterator iter; for (iter = x.begin(); iter != x.end(); iter++) { delete *iter; } }
#define FREE_OBJECT(x) { JSONObject::iterator iter; for (iter = x.begin(); iter != x.end(); iter++) { delete (*iter).second; } }

/**
 * Parses a JSON encoded value to a JSONValue object
 *
 * @access protected
 *
 * @param wchar_t** data Pointer to a wchar_t* that contains the data
 *
 * @return JSONValue* Returns a pointer to a JSONValue object on success, NULL on error
 */
JSONValue* JSONValue::Parse( const wchar_t * *data )
{
    // Is it a string?
    if ( **data == '"' )
    {
        std::wstring str;
        if ( !JSON::ExtractString( &( ++( *data ) ), str ) )
        { return NULL; }
        else
        { return new JSONValue( str ); }
    }
    
    // Is it a boolean?
    else if ( ( simplejson_wcsnlen( *data, 4 ) && wcsncasecmp( *data, L"true", 4 ) == 0 ) || ( simplejson_wcsnlen( *data, 5 ) && wcsncasecmp( *data, L"false", 5 ) == 0 ) )
    {
        bool value = wcsncasecmp( *data, L"true", 4 ) == 0;
        ( *data ) += value ? 4 : 5;
        return new JSONValue( value );
    }
    
    // Is it a null?
    else if ( simplejson_wcsnlen( *data, 4 ) && wcsncasecmp( *data, L"null", 4 ) == 0 )
    {
        ( *data ) += 4;
        return new JSONValue();
    }
    
    // Is it a number?
    else if ( **data == L'-' || ( **data >= L'0' &&** data <= L'9' ) )
    {
        // Negative?
        bool neg = **data == L'-';
        if ( neg ) { ( *data )++; }
        
        double number = 0.0;
        
        // Parse the whole part of the number - only if it wasn't 0
        if ( **data == L'0' )
        { ( *data )++; }
        else if ( **data >= L'1' &&** data <= L'9' )
        { number = JSON::ParseInt( data ); }
        else
        { return NULL; }
        
        // Could be a decimal now...
        if ( **data == '.' )
        {
            ( *data )++;
            
            // Not get any digits?
            if ( !( **data >= L'0' &&** data <= L'9' ) )
            { return NULL; }
            
            // Find the decimal and sort the decimal place out
            // Use ParseDecimal as ParseInt won't work with decimals less than 0.1
            // thanks to Javier Abadia for the report & fix
            double decimal = JSON::ParseDecimal( data );
            
            // Save the number
            number += decimal;
        }
        
        // Could be an exponent now...
        if ( **data == L'E' ||** data == L'e' )
        {
            ( *data )++;
            
            // Check signage of expo
            bool neg_expo = false;
            if ( **data == L'-' ||** data == L'+' )
            {
                neg_expo = **data == L'-';
                ( *data )++;
            }
            
            // Not get any digits?
            if ( !( **data >= L'0' &&** data <= L'9' ) )
            { return NULL; }
            
            // Sort the expo out
            double expo = JSON::ParseInt( data );
            for ( double i = 0.0; i < expo; i++ )
            { number = neg_expo ? ( number / 10.0 ) : ( number * 10.0 ); }
        }
        
        // Was it neg?
        if ( neg ) { number *= -1; }
        
        return new JSONValue( number );
    }
    
    // An object?
    else if ( **data == L'{' )
    {
        JSONObject object;
        
        ( *data )++;
        
        while ( **data != 0 )
        {
            // Whitespace at the start?
            if ( !JSON::SkipWhitespace( data ) )
            {
                FREE_OBJECT( object );
                return NULL;
            }
            
            // Special case - empty object
            if ( object.size() == 0 &&** data == L'}' )
            {
                ( *data )++;
                return new JSONValue( object );
            }
            
            // We want a string now...
            std::wstring name;
            if ( !JSON::ExtractString( &( ++( *data ) ), name ) )
            {
                FREE_OBJECT( object );
                return NULL;
            }
            
            // More whitespace?
            if ( !JSON::SkipWhitespace( data ) )
            {
                FREE_OBJECT( object );
                return NULL;
            }
            
            // Need a : now
            if ( *( ( *data )++ ) != L':' )
            {
                FREE_OBJECT( object );
                return NULL;
            }
            
            // More whitespace?
            if ( !JSON::SkipWhitespace( data ) )
            {
                FREE_OBJECT( object );
                return NULL;
            }
            
            // The value is here
            JSONValue* value = Parse( data );
            if ( value == NULL )
            {
                FREE_OBJECT( object );
                return NULL;
            }
            
            // Add the name:value
            if ( object.find( name ) != object.end() )
            { delete object[name]; }
            object[name] = value;
            
            // More whitespace?
            if ( !JSON::SkipWhitespace( data ) )
            {
                FREE_OBJECT( object );
                return NULL;
            }
            
            // End of object?
            if ( **data == L'}' )
            {
                ( *data )++;
                return new JSONValue( object );
            }
            
            // Want a , now
            if ( **data != L',' )
            {
                FREE_OBJECT( object );
                return NULL;
            }
            
            ( *data )++;
        }
        
        // Only here if we ran out of data
        FREE_OBJECT( object );
        return NULL;
    }
    
    // An array?
    else if ( **data == L'[' )
    {
        JSONArray array;
        
        ( *data )++;
        
        while ( **data != 0 )
        {
            // Whitespace at the start?
            if ( !JSON::SkipWhitespace( data ) )
            {
                FREE_ARRAY( array );
                return NULL;
            }
            
            // Special case - empty array
            if ( array.size() == 0 &&** data == L']' )
            {
                ( *data )++;
                return new JSONValue( array );
            }
            
            // Get the value
            JSONValue* value = Parse( data );
            if ( value == NULL )
            {
                FREE_ARRAY( array );
                return NULL;
            }
            
            // Add the value
            array.push_back( value );
            
            // More whitespace?
            if ( !JSON::SkipWhitespace( data ) )
            {
                FREE_ARRAY( array );
                return NULL;
            }
            
            // End of array?
            if ( **data == L']' )
            {
                ( *data )++;
                return new JSONValue( array );
            }
            
            // Want a , now
            if ( **data != L',' )
            {
                FREE_ARRAY( array );
                return NULL;
            }
            
            ( *data )++;
        }
        
        // Only here if we ran out of data
        FREE_ARRAY( array );
        return NULL;
    }
    
    // Ran out of possibilites, it's bad!
    else
    {
        return NULL;
    }
}

/**
 * Basic constructor for creating a JSON Value of m_type NULL
 *
 * @access public
 */
JSONValue::JSONValue( /*NULL*/ )
{
    m_type = JSONType_Null;
}

/**
 * Basic constructor for creating a JSON Value of type String
 *
 * @access public
 *
 * @param wchar_t* m_char_value The string to use as the value
 */
JSONValue::JSONValue( const wchar_t* char_value )
{
    m_type = JSONType_String;
    m_string_value = std::wstring( char_value );
}

/**
 * Basic constructor for creating a JSON Value of type String
 *
 * @access public
 *
 * @param std::wstring m_string_value The string to use as the value
 */
JSONValue::JSONValue( const std::wstring& string_value )
{
    m_type = JSONType_String;
    m_string_value = string_value;
}

/**
 * Basic constructor for creating a JSON Value of m_type Bool
 *
 * @access public
 *
 * @param bool m_bool_value The bool to use as the value
 */
JSONValue::JSONValue( bool bool_value )
{
    m_type = JSONType_Bool;
    m_bool_value = bool_value;
}

/**
 * Basic constructor for creating a JSON Value of type Number
 *
 * @access public
 *
 * @param double m_number_value The number to use as the value
 */
JSONValue::JSONValue( double number_value )
{
    m_type = JSONType_Number;
    m_number_value = number_value;
}

/**
 * Basic constructor for creating a JSON Value of type Array
 *
 * @access public
 *
 * @param JSONArray m_array_value The JSONArray to use as the value
 */
JSONValue::JSONValue( const JSONArray& array_value )
{
    m_type = JSONType_Array;
    m_array_value = array_value;
}

/**
 * Basic constructor for creating a JSON Value of type Object
 *
 * @access public
 *
 * @param JSONObject m_object_value The JSONObject to use as the value
 */
JSONValue::JSONValue( const JSONObject& object_value )
{
    m_type = JSONType_Object;
    m_object_value = object_value;
}

/**
 * The destructor for the JSON Value object
 * Handles deleting the objects in the array or the object value
 *
 * @access public
 */
JSONValue::~JSONValue()
{
    if ( m_type == JSONType_Array )
    {
        JSONArray::iterator iter;
        for ( iter = m_array_value.begin(); iter != m_array_value.end(); iter++ )
        { delete *iter; }
    }
    else if ( m_type == JSONType_Object )
    {
        JSONObject::iterator iter;
        for ( iter = m_object_value.begin(); iter != m_object_value.end(); iter++ )
        {
            delete ( *iter ).second;
        }
    }
}

/**
 * Checks if the value is a NULL
 *
 * @access public
 *
 * @return bool Returns true if it is a NULL value, false otherwise
 */
bool JSONValue::IsNull() const
{
    return m_type == JSONType_Null;
}

/**
 * Checks if the value is a String
 *
 * @access public
 *
 * @return bool Returns true if it is a String value, false otherwise
 */
bool JSONValue::IsString() const
{
    return m_type == JSONType_String;
}

/**
 * Checks if the value is a Bool
 *
 * @access public
 *
 * @return bool Returns true if it is a Bool value, false otherwise
 */
bool JSONValue::IsBool() const
{
    return m_type == JSONType_Bool;
}

/**
 * Checks if the value is a Number
 *
 * @access public
 *
 * @return bool Returns true if it is a Number value, false otherwise
 */
bool JSONValue::IsNumber() const
{
    return m_type == JSONType_Number;
}

/**
 * Checks if the value is an Array
 *
 * @access public
 *
 * @return bool Returns true if it is an Array value, false otherwise
 */
bool JSONValue::IsArray() const
{
    return m_type == JSONType_Array;
}

/**
 * Checks if the value is an Object
 *
 * @access public
 *
 * @return bool Returns true if it is an Object value, false otherwise
 */
bool JSONValue::IsObject() const
{
    return m_type == JSONType_Object;
}

/**
 * Retrieves the String value of this JSONValue
 * Use IsString() before using this method.
 *
 * @access public
 *
 * @return std::wstring Returns the string value
 */
const std::wstring& JSONValue::AsString() const
{
    return m_string_value;
}

/**
 * Retrieves the Bool value of this JSONValue
 * Use IsBool() before using this method.
 *
 * @access public
 *
 * @return bool Returns the bool value
 */
bool JSONValue::AsBool() const
{
    return m_bool_value;
}

/**
 * Retrieves the Number value of this JSONValue
 * Use IsNumber() before using this method.
 *
 * @access public
 *
 * @return double Returns the number value
 */
double JSONValue::AsNumber() const
{
    return m_number_value;
}


/**
 *
 *
 *
 * @access public
 *
 * @return
 */

bool JSONValue::AsType( const wchar_t* name, JSONVType m_type, void* data )
{
    std::wstring str = name;
    switch( m_type )
    {
    case JSONVType_String:
        {
            if ( m_object_value.find( str ) != m_object_value.end() && m_object_value[str]->IsString() )
            {
                ( *( std::wstring* )data ) = m_object_value[str]->AsString();
                return true;
            }
        }
        break;
    case JSONVType_Bool:
        {
            if ( m_object_value.find( str ) != m_object_value.end() && m_object_value[str]->IsBool() )
            {
                ( *( bool* )data ) = m_object_value[str]->AsBool();
                return true;
            }
        }
        break;
    case JSONVType_Double:
        {
            if ( m_object_value.find( str ) != m_object_value.end() && m_object_value[str]->IsNumber() )
            {
                ( *( ( double* )data ) ) = m_object_value[str]->AsNumber();
                return true;
            }
        }
        break;
    case JSONVType_Float:
        {
            if ( m_object_value.find( str ) != m_object_value.end() && m_object_value[str]->IsNumber() )
            {
                ( *( ( float* )data ) ) = ( float )m_object_value[str]->AsNumber();
                return true;
            }
        }
        break;
    case JSONVType_LLong:
        {
            if ( m_object_value.find( str ) != m_object_value.end() && m_object_value[str]->IsNumber() )
            {
                ( *( ( long64_t* )data ) ) = ( long64_t )m_object_value[str]->AsNumber();
                return true;
            }
        }
        break;
    case JSONVType_Long:
        {
            if ( m_object_value.find( str ) != m_object_value.end() && m_object_value[str]->IsNumber() )
            {
                ( *( ( int* )data ) ) = ( int )m_object_value[str]->AsNumber();
                return true;
            }
        }
        break;
    case JSONVType_Short:
        {
            if ( m_object_value.find( str ) != m_object_value.end() && m_object_value[str]->IsNumber() )
            {
                ( *( ( short* )data ) ) = ( short )m_object_value[str]->AsNumber();
                return true;
            }
        }
        break;
    }
    return false;
}

/**
 * Retrieves the Array value of this JSONValue
 * Use IsArray() before using this method.
 *
 * @access public
 *
 * @return JSONArray Returns the array value
 */
const JSONArray& JSONValue::AsArray() const
{
    return m_array_value;
}

/**
 * Retrieves the Object value of this JSONValue
 * Use IsObject() before using this method.
 *
 * @access public
 *
 * @return JSONObject Returns the object value
 */
const JSONObject& JSONValue::AsObject() const
{
    return m_object_value;
}

/**
 * Creates a JSON encoded string for the value with all necessary characters escaped
 *
 * @access public
 *
 * @return std::wstring Returns the JSON string
 */
std::wstring JSONValue::Stringify() const
{
    std::wstring ret_string;
    
    switch ( m_type )
    {
    case JSONType_Null:
        ret_string = L"null";
        break;
        
    case JSONType_String:
        ret_string = StringifyString( m_string_value );
        break;
        
    case JSONType_Bool:
        ret_string = m_bool_value ? L"true" : L"false";
        break;
        
    case JSONType_Number:
        {
            if ( isinf( m_number_value ) || isnan( m_number_value ) )
            { ret_string = L"null"; }
            else
            {
                std::wstringstream ss;
                ss << m_number_value;
                ret_string = ss.str();
            }
            break;
        }
        
    case JSONType_Array:
        {
            ret_string = L"[";
            JSONArray::const_iterator iter = m_array_value.begin();
            while ( iter != m_array_value.end() )
            {
                ret_string += ( *iter )->Stringify();
                
                // Not at the end - add a separator
                if ( ++iter != m_array_value.end() )
                { ret_string += L","; }
            }
            ret_string += L"]";
            break;
        }
        
    case JSONType_Object:
        {
            ret_string = L"{";
            JSONObject::const_iterator iter = m_object_value.begin();
            while ( iter != m_object_value.end() )
            {
                ret_string += StringifyString( ( *iter ).first );
                ret_string += L":";
                ret_string += ( *iter ).second->Stringify();
                
                // Not at the end - add a separator
                if ( ++iter != m_object_value.end() )
                { ret_string += L","; }
            }
            ret_string += L"}";
            break;
        }
    }
    
    return ret_string;
}


bool JSONValue::TestJson( const std::string& str )
{
    if ( ( str[ 0 ] == '{' ) && ( str.back() == '}' ) )
    {
        return true;
    }

    if ( ( str[ 0 ] == '[' ) && ( str.back() == ']' ) )
    {
        return true;
    }
    return false;
}

bool JSONValue::TestJson( const std::wstring& str )
{
    if ( ( str[ 0 ] == '{' ) && ( str.back() == '}' ) )
    {
        return true;
    }

    if ( ( str[ 0 ] == '[' ) && ( str.back() == ']' ) )
    {
        return true;
    }
    return false;
}


/**
 * Creates a JSON encoded string with all required fields escaped
 * Works from http://www.ecma-internationl.org/publications/files/ECMA-ST/ECMA-262.pdf
 * Section 15.12.3.
 *
 * @access private
 *         public : Pin
 * @param std::wstring str The string that needs to have the characters escaped
 *
 * @return std::wstring Returns the JSON string

 */

std::string JSONValue::StringifyString( const std::string& str )
{
    std::string str_out = "\"";

    std::string::const_iterator iter = str.begin();
    while ( iter != str.end() )
    {
        char chr = *iter;

        if ( chr == '"' || chr == '\\' || chr == '/' )
        {
            str_out += '\\';
            str_out += chr;
        }
        else if ( chr == '\b' )
        {
            str_out += "\\b";
        }
        else if ( chr == '\f' )
        {
            str_out += "\\f";
        }
        else if ( chr == '\n' )
        {
            str_out += "\\n";
        }
        else if ( chr == '\r' )
        {
            str_out += "\\r";
        }
        else if ( chr == '\t' )
        {
            str_out += "\\t";
        }
        //else if ( chr < ' ' )
        //{
        //    str_out += "\\u";
        //    for ( int i = 0; i < 4; i++ )
        //    {
        //        int value = ( chr >> 12 ) & 0xf;
        //        if ( value >= 0 && value <= 9 )
        //        {
        //            str_out += ( char )( '0' + value );
        //        }
        //        else if ( value >= 10 && value <= 15 )
        //        {
        //            str_out += ( char )( 'A' + ( value - 10 ) );
        //        }
        //        chr <<= 4;
        //    }
        //}
        else
        {
            str_out += chr;
        }

        iter++;
    }

    str_out += "\"";
    return str_out;
}


std::wstring JSONValue::StringifyString( const std::wstring& str )
{
    std::wstring str_out = L"\"";
    
    std::wstring::const_iterator iter = str.begin();
    while ( iter != str.end() )
    {
        wchar_t chr = *iter;
        
        if ( chr == L'"' || chr == L'\\' || chr == L'/' )
        {
            str_out += L'\\';
            str_out += chr;
        }
        else if ( chr == L'\b' )
        {
            str_out += L"\\b";
        }
        else if ( chr == L'\f' )
        {
            str_out += L"\\f";
        }
        else if ( chr == L'\n' )
        {
            str_out += L"\\n";
        }
        else if ( chr == L'\r' )
        {
            str_out += L"\\r";
        }
        else if ( chr == L'\t' )
        {
            str_out += L"\\t";
        }
        else if ( chr < L' ' )
        {
            str_out += L"\\u";
            for ( int i = 0; i < 4; i++ )
            {
                int value = ( chr >> 12 ) & 0xf;
                if ( value >= 0 && value <= 9 )
                { 
                    str_out += ( wchar_t )( '0' + value ); 
                }
                else if ( value >= 10 && value <= 15 )
                { 
                    str_out += ( wchar_t )( 'A' + ( value - 10 ) ); 
                }
                chr <<= 4;
            }
        }
        else
        {
            str_out += chr;
        }
        
        iter++;
    }
    
    str_out += L"\"";
    return str_out;
}
