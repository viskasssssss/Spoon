#pragma once

namespace spoon {
    enum call_code {
        // 000 -> 100 - temp / debug / unstable calls
        // 100 -> 200 - system / console calls
        // 200 -> 300 - UI calls
        // 300 -> 400 - detail / window calls for creating/controlling windows
        // 400 -> 500 - rendering calls
        // 500 -> 600 - app specific calls

        CALL_CODE_PRINT = 101,

        CALL_CODE_UI_SET_WINDOW = 201,
        CALL_CODE_UI_POP = 202,
        CALL_CODE_UI_START_WINDOW =  203,
        CALL_CODE_UI_BASIC_INSTRUCTION = 204,

        CALL_CODE_WINDOW_GET_MAIN = 301,

    };
}

