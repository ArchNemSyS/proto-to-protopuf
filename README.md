# proto-to-protopuf
A parser for Google proto source files to native compatible [protopuf](https://github.com/PragmaTwice/protopuf) C++ headers 

## limitations
- only supports proto version 2 files - (should be relative easy to overload)
- ignores namespace package
- tokenizer is confused by comment looking string literals aka - file paths
- designed to be used as a batch/shell program, wrap it in a script or alter main to recurse over many files
- doesn't generate make files etc



## supported tags

### COMMENTS

- /* multiline find */
- // singleline
 
blind copy and paste, also in codeblocks


### import

    import "filename.proto";

--convert--

    include "filename.proto.h"

### enum

    enum TYPE {
        FIELD_NAME = INT;
        FIELD_NAME = INT;
    }

--convert--

    enum TYPE {
       FIELD_NAME = INT,
       FIELD_NAME = INT,
    }


### message [TODO]
    message Student {
        uint32 id = 1;
        string name = 3;
    }

    message Class {
        string name = 8;
        repeated Student students = 3;
    }

--convert--

    using Student = message<
        uint32_field<"id", 1>, 
        string_field<"name", 3>
    >;
    
    using Class = message<
        string_field<"name", 8>, 
        message_field<"students", 3, Student, repeated>
    >;

