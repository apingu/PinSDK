// At first you need to include header file
#include <MPDFParser-0.1/Parser.h>

int main(int argc, char** argv) {
    // Here comes your own preparation, looping,
    // connection waiting or something else.

    try {
        // Create parser object
        MPFD::Parser parser;

        // Set directory for temp files of something is uploaded
        parser.SetTempDirForFileUpload("/tmp");

        // Pass Content-type value. If Multipart/form-data incomes,
        // it will also contain boundary= section

        std::string content_type;
        // Set content type variable here

        parser.setContentType(content_type);

        // Create buffer to transfer POST data
        const int ReadBufferSize = 4 * 1024;
        char input[ReadBufferSize];

        // Now read incoming data to buffer part by part
        // and transfer it to parser.
        do {

            // How many bytes were read
            int read;

            // Here you need to fill the buffer

            if (read) {
                parser.AcceptSomeData(input, read);
            }

        } while (/*until input data ends*/);


        // Now you have map of fields
        std::map <std::string,MPFD::Field *> fields=p.GetFieldsMap();

        // For example "name" and "surname" text fields where submitted:
        std::string name=fields["name"]->GetTextTypeContent();

        std::string surname=fields["surname"]->GetTextTypeContent();

        // And photo was uploaded:
        std::string photo_temp_file=fields["photo"]->GetTempFileName();

        // All is done

    } catch (MPFD::Exception e) {
        // Log error at this point.
        // e.GetError() will tell you what happened.
    }

    // At this point parser and all it's fields were destructed, temp files
    // deleted and memory cleared

}
