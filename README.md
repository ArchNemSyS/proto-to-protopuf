# proto-to-protopuf
A simple parser for Google proto source files to [protopuf](https://github.com/PragmaTwice/protopuf) C++ headers 
- default usage is to convert all proto sources in one directory into protopuf headers

## Limitations
- only supports proto version 2 files and common field types - (should be relatively easy to overload)
- supports includes but cannot resolve enums and messages with the same name
- doesnt resolve symbol dependancy order - ie forward declares / reordering of declarations 
- ignores namespace/package attributes
- tokenizer is confused by comment looking string literals aka - file paths // and /*
- doesn't generate make files or wrapper classes etc

## Supported tags

### message fields
| proto  | protopuf |
| ------------- | ------------- |
|int32   | int32_field |
|unit32  | uint32_field |
|int64   | int64_field |
|uint64  | uint64_field |
| |
|float   | float_field |
|double  | double_field |
| |
|string  | string_field |
| |
|bool    | bool_field |
|enum |enum_field |
|message |message_field |


### comments
- /* multiline */
- // singleline
 

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

