/*
 * File json_value.h part of the SimpleJSON Library - http://mjpa.in/json
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

#ifndef _JSONVALUE_H_
#define _JSONVALUE_H_

#include <vector>
#include <string>

#include "json.h"

class JSON;

enum JSONType
{
    JSONType_Null, JSONType_String, JSONType_Bool, JSONType_Number, JSONType_Array, JSONType_Object
};

enum JSONVType
{
    JSONVType_String, JSONVType_Bool, JSONVType_Double, JSONVType_Float, JSONVType_LLong, JSONVType_Long, JSONVType_Short
};


class JSONValue
{
    friend class JSON;
    
public:
    JSONValue( /*NULL*/ );
    JSONValue( const wchar_t* char_value );
    JSONValue( const std::wstring& string_value );
    JSONValue( bool bool_value );
    JSONValue( double number_value );
    JSONValue( const JSONArray& array_value );
    JSONValue( const JSONObject& object_value );
    ~JSONValue();
    
    bool IsNull() const;
    bool IsString() const;
    bool IsBool() const;
    bool IsNumber() const;
    bool IsArray() const;
    bool IsObject() const;
    
    const std::wstring& AsString() const;
    bool AsBool() const;
    double AsNumber() const;
    // set data by type
    bool AsType( const wchar_t* name, JSONVType type, void* data );
    
    const JSONArray& AsArray() const;
    const JSONObject& AsObject() const;
   
    std::wstring Stringify() const;

    static bool TestJson( const std::string& str );
    static bool TestJson( const std::wstring& str );

    static std::string StringifyString( const std::string& str );
    static std::wstring StringifyString( const std::wstring& str );

    
protected:
    static JSONValue* Parse( const wchar_t * *data );
    
private:
    
    JSONType     m_type;
    std::wstring m_string_value;
    bool         m_bool_value;
    double       m_number_value;
    JSONArray    m_array_value;
    JSONObject   m_object_value;
};

#endif
