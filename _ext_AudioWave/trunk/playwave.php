<html>

<!-- 
    nradiowave clock "emulator" page for test animations & firmwave for LED-clock
-->

<head>
<title>Portal OST - Radio.mp3</title>
<style>
body, html {
    margin: 0px;
    padding: 0px; 
    
    font-size : 16px;
    
    font-family: Arial, Helvetica, sans-serif;
    background: #3b3c3f;
    color: #e7e7e7;
    width: 100%;
    height: 100%;
}

button {
    background: #4c4c4c;
    border: 0;
    padding: 6px;
    color: #fff;
    margin-top: 20px;
}

#screen {

    padding-top : 64px;
    text-align : center;
}     
     
#screen table {
    display : inline-block;
}
     
#screen td {
    width: 20px;
    height: 20px;
    min-height: 20px;
    min-width: 20px;
    overflow: hidden;
    background: #222;
    text-align: center;
    padding: 0;
    margin: 0;
    border: 0;
    box-sizing: border-box;
    border-radius: 2px;
}

#screen td.active {
    background: #ff9200;
    box-shadow: 0 0 11px 3px #ff6c009e;
}

#buttons {
    text-align : center;
    
}

#font-src {
    display: block;
    width: 100%;
    min-height: 300px
}

.form-default {
    padding : 6px;
    display : block;
    border : 1px solid #fff;  
    max-width: 600px;
    margin: 0 auto;
    margin-top: 16px;
}

.form-default textarea {
    display: block;
    width: 100%;
    height: 600px;
}
</style>


</head>

<body>
<div id="screen"></div>
<div id="buttons">

<input type="text" id="text-data" style="display : none" value="Съешь ещё этих мягких французских булок, да выпей чаю 1234567890"  class="form-data">


<button id="test-scroll">Test scroll</button>
<button id="test-clock">Test clock</button>
<button id="test-audio">Test audio</button>
<button id="show-font-src">Font to C++</button>
<button id="image-to-src">File to C++ bin-array</button>

<!--button id="test-convert">Test convert</button-->
<!--<button id="left">left</button><button id="right">right</button><button id="loopX">loopX</button><button id="toBottom">toBottom</button>-->

<textarea id="font-src" style="display : none" class="form-data"></textarea>

<div style="display : none" id="image-to-src-form" class="form-default form-data">
    <input type="file" id="image-to-src-file">
    <button id="image-to-src-run">Convert</button>
    <textarea id="image-to-src-result" style="display : none"></textarea>
    <div id="image-to-src-error"></div>
</div>

</div>
<script>
var font8x8 = [
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // space
      0x00, 0x00, 0x00, 0x40, 0xC0, 0x40, 0x40, 0x40,  // 1
      0x00, 0x00, 0x00, 0xE0, 0x20, 0xE0, 0x80, 0xE0,  // 2
      0x00, 0x00, 0x00, 0xE0, 0x20, 0xE0, 0x20, 0xE0,  // 3
      0x00, 0x00, 0x00, 0xA0, 0xA0, 0xE0, 0x20, 0x20,  // 4
      0x00, 0x00, 0x00, 0xE0, 0x80, 0xE0, 0x20, 0xE0,  // 5
      0x00, 0x00, 0x00, 0xE0, 0x80, 0xE0, 0xA0, 0xE0,  // 6
      0x00, 0x00, 0x00, 0xE0, 0x20, 0x20, 0x20, 0x20,  // 7
      0x00, 0x00, 0x00, 0xE0, 0xA0, 0xE0, 0xA0, 0xE0,  // 8
      0x00, 0x00, 0x00, 0xE0, 0xA0, 0xE0, 0x20, 0xE0,  // 9
      0x00, 0x00, 0x00, 0xE0, 0xA0, 0xA0, 0xA0, 0xE0,  // 0
      0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x80,  // !
      0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,  // :
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,  // .
      0x00, 0x00, 0x00, 0xC8, 0xD0, 0x20, 0x58, 0x98,  // %
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x10,  // ,
      0x00, 0x00, 0x00, 0x00, 0x40, 0xE0, 0x40, 0x00,  // +
      0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00,  // -
      0x00, 0x00, 0x00, 0x40, 0x80, 0x80, 0x80, 0x40,  // (
      0x00, 0x00, 0x00, 0x80, 0x40, 0x40, 0x40, 0x80,  // )
      0x00, 0x00, 0x40, 0xA0, 0x20, 0x40, 0x00, 0x40,  // ?
      0x00, 0x00, 0x00, 0xC0, 0x80, 0x80, 0x80, 0xC0,  // [
      0x00, 0x00, 0x00, 0xC0, 0x40, 0x40, 0x40, 0xC0,  // ]
      0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00,  // '
      0x00, 0x00, 0x00, 0xA0, 0xA0, 0x00, 0x00, 0x00,  // "
      0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0xE0, 0x00,  // =
      0x00, 0x00, 0x40, 0xA0, 0x00, 0x00, 0x00, 0x00,  // ^
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0,  // _
      0x00, 0x00, 0x00, 0xE0, 0x20, 0xE0, 0xA0, 0xF0,  // а
      0x00, 0x00, 0x00, 0xE0, 0x80, 0xE0, 0xA0, 0xE0,  // б
      0x00, 0x00, 0x00, 0xC0, 0xA0, 0xE0, 0xA0, 0xE0,  // в
      0x00, 0x00, 0x00, 0xE0, 0x80, 0x80, 0x80, 0x80,  // г
      0x00, 0x00, 0x00, 0xE0, 0xA0, 0xE0, 0x20, 0xC0,  // д
      0x00, 0x00, 0x00, 0xE0, 0xA0, 0xE0, 0x80, 0xE0,  // е
      0x00, 0x00, 0xA0, 0xE0, 0xA0, 0xE0, 0x80, 0xE0,  // ё
      0x00, 0x00, 0x00, 0xA8, 0x70, 0x20, 0x70, 0x88,  // ж
      0x00, 0x00, 0x00, 0xC0, 0x20, 0x40, 0x20, 0xC0,  // з
      0x00, 0x00, 0x00, 0x90, 0x90, 0xB0, 0xD0, 0x90,  // и
      0x00, 0x00, 0x60, 0x00, 0x90, 0xB0, 0xD0, 0x90,  // й
      0x00, 0x00, 0x00, 0x90, 0xA0, 0xC0, 0xA0, 0x90,  // к
      0x00, 0x00, 0x00, 0x20, 0x50, 0x50, 0x50, 0x90,  // л
      0x00, 0x00, 0x00, 0x88, 0xD8, 0xA8, 0x88, 0x88,  // м
      0x00, 0x00, 0x00, 0x90, 0x90, 0xF0, 0x90, 0x90,  // н
      0x00, 0x00, 0x00, 0xE0, 0xA0, 0xA0, 0xA0, 0xE0,  // о
      0x00, 0x00, 0x00, 0xF0, 0x90, 0x90, 0x90, 0x90,  // п
      0x00, 0x00, 0x00, 0xE0, 0xA0, 0xE0, 0x80, 0x80,  // р
      0x00, 0x00, 0x00, 0x60, 0x80, 0x80, 0x80, 0x60,  // с
      0x00, 0x00, 0x00, 0xE0, 0x40, 0x40, 0x40, 0x40,  // т
      0x00, 0x00, 0xA0, 0xA0, 0xE0, 0x20, 0x20, 0xC0,  // у
      0x00, 0x00, 0x00, 0x70, 0xA8, 0x70, 0x20, 0x20,  // ф
      0x00, 0x00, 0x00, 0x90, 0x90, 0x60, 0x90, 0x90,  // х
      0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0xF8, 0x08,  // ц
      0x00, 0x00, 0x00, 0xA0, 0xA0, 0x60, 0x20, 0x20,  // ч
      0x00, 0x00, 0x00, 0xA8, 0xA8, 0xA8, 0xA8, 0xF8,  // ш
      0x00, 0x00, 0xA8, 0xA8, 0xA8, 0xA8, 0xF8, 0x08,  // щ
      0x00, 0x00, 0x00, 0xC0, 0x40, 0x70, 0x50, 0x70,  // ъ
      0x00, 0x00, 0x00, 0x80, 0x80, 0xE0, 0xA0, 0xE0,  // ь
      0x00, 0x00, 0x00, 0x88, 0x88, 0xE8, 0xA8, 0xE8,  // ы
      0x00, 0x00, 0x00, 0xC0, 0x20, 0xE0, 0x20, 0xC0,  // э
      0x00, 0x00, 0x00, 0x90, 0xA8, 0xE8, 0xA8, 0x90,  // ю
      0x00, 0x00, 0x00, 0xE0, 0xA0, 0xE0, 0x60, 0xA0,  // я
      0x00, 0x00, 0x60, 0x90, 0x90, 0xF0, 0x90, 0x90,  // А
      0x00, 0x00, 0xF0, 0x80, 0xF0, 0x90, 0x90, 0xF0,  // Б
      0x00, 0x00, 0xE0, 0x90, 0xE0, 0x90, 0x90, 0xF0,  // В
      0x00, 0x00, 0xF0, 0x80, 0x80, 0x80, 0x80, 0x80,  // Г
      0x00, 0x00, 0x70, 0x50, 0x50, 0x50, 0xF8, 0x88,  // Д
      0x00, 0x00, 0xF0, 0x80, 0xF0, 0x80, 0x80, 0xF0,  // Е
      0x00, 0x00, 0x50, 0xF0, 0x80, 0xF0, 0x80, 0xF0,  // Ё
      0x00, 0x00, 0xA8, 0xA8, 0x70, 0x70, 0xA8, 0xA8,  // Ж
      0x00, 0x00, 0xE0, 0x10, 0x60, 0x10, 0x10, 0xE0,  // З
      0x00, 0x00, 0x98, 0x98, 0xA8, 0xA8, 0xC8, 0xC8,  // И
      0x00, 0x00, 0xA8, 0x88, 0x98, 0xA8, 0xA8, 0xC8,  // Й
      0x00, 0x00, 0x98, 0x90, 0xE0, 0x90, 0x90, 0x98,  // К
      0x00, 0x00, 0x38, 0x48, 0x48, 0x48, 0x48, 0x88,  // Л
      0x00, 0x00, 0x88, 0xD8, 0xA8, 0x88, 0x88, 0x88,  // М
      0x00, 0x00, 0x88, 0x88, 0x88, 0xF8, 0x88, 0x88,  // Н
      0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x88, 0x70,  // О
      0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x88, 0x88,  // П
      0x00, 0x00, 0x70, 0x88, 0x88, 0xF0, 0x80, 0x80,  // Р
      0x00, 0x00, 0x70, 0x88, 0x80, 0x80, 0x88, 0x70,  // С
      0x00, 0x00, 0xF8, 0xA8, 0x20, 0x20, 0x20, 0x20,  // Т
      0x00, 0x00, 0x88, 0x88, 0x88, 0x78, 0x08, 0x70,  // У
      0x00, 0x00, 0x70, 0xA8, 0xA8, 0x70, 0x20, 0x20,  // Ф
      0x00, 0x00, 0x90, 0x90, 0x60, 0x60, 0x90, 0x90,  // Х
      0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0xF8, 0x08,  // Ц
      0x00, 0x00, 0x90, 0x90, 0x90, 0xF0, 0x10, 0x10,  // Ч
      0x00, 0x00, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xF8,  // Ш
      0x00, 0x00, 0xA8, 0xA8, 0xA8, 0xA8, 0xF0, 0x08,  // Щ
      0x00, 0x00, 0xC0, 0x40, 0x78, 0x48, 0x48, 0x78,  // Ъ
      0x00, 0x00, 0x80, 0x80, 0xF0, 0x90, 0x90, 0xF0,  // Ь
      0x00, 0x00, 0x88, 0x88, 0xE8, 0xA8, 0xA8, 0xE8,  // Ы
      0x00, 0x00, 0xE0, 0x90, 0x10, 0x30, 0x90, 0xE0,  // Э
      0x00, 0x00, 0xB8, 0xA8, 0xA8, 0xE8, 0xA8, 0xB8,  // Ю
      0x00, 0x00, 0xF0, 0x90, 0x90, 0xF0, 0x50, 0x90,  // Я
      0x00, 0x00, 0x00, 0xC0, 0x20, 0xE0, 0xA0, 0xD0,  // a
      0x00, 0x00, 0x00, 0x80, 0x80, 0xE0, 0xA0, 0xE0,  // b
      0x00, 0x00, 0x00, 0x60, 0x80, 0x80, 0x80, 0x60,  // c
      0x00, 0x00, 0x00, 0x20, 0x20, 0xE0, 0xA0, 0xE0,  // d
      0x00, 0x00, 0x00, 0xE0, 0xA0, 0xE0, 0x80, 0xE0,  // e
      0x00, 0x00, 0x00, 0x60, 0x40, 0xE0, 0x40, 0x40,  // f
      0x00, 0x00, 0x00, 0xE0, 0xA0, 0xE0, 0x20, 0xC0,  // g
      0x00, 0x00, 0x00, 0x80, 0x80, 0xE0, 0xA0, 0xA0,  // h
      0x00, 0x00, 0x00, 0x40, 0x00, 0x40, 0x40, 0x40,  // i
      0x00, 0x00, 0x40, 0x00, 0x40, 0x40, 0x40, 0x80,  // j
      0x00, 0x00, 0x00, 0x80, 0xA0, 0xC0, 0xA0, 0xA0,  // k
      0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40,  // l
      0x00, 0x00, 0x00, 0x50, 0xA8, 0xA8, 0xA8, 0xA8,  // m
      0x00, 0x00, 0x00, 0xE0, 0x90, 0x90, 0x90, 0x90,  // n
      0x00, 0x00, 0x00, 0xE0, 0xA0, 0xA0, 0xA0, 0xE0,  // o
      0x00, 0x00, 0x00, 0xE0, 0xA0, 0xE0, 0x80, 0x80,  // p
      0x00, 0x00, 0x00, 0xE0, 0xA0, 0xE0, 0x20, 0x20,  // q
      0x00, 0x00, 0x00, 0x60, 0x90, 0x80, 0x80, 0x80,  // r
      0x00, 0x00, 0x00, 0xE0, 0x80, 0xE0, 0x20, 0xE0,  // s
      0x00, 0x00, 0x00, 0x40, 0xE0, 0x40, 0x40, 0x60,  // t
      0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x60,  // u
      0x00, 0x00, 0x00, 0xA0, 0xA0, 0xA0, 0xA0, 0x40,  // v
      0x00, 0x00, 0x00, 0x88, 0xA8, 0xA8, 0x50, 0x50,  // w
      0x00, 0x00, 0x00, 0x90, 0x90, 0x60, 0x90, 0x90,  // x
      0x00, 0x00, 0xA0, 0xA0, 0xE0, 0x20, 0x20, 0xC0,  // y
      0x00, 0x00, 0x00, 0xF0, 0x10, 0x60, 0x80, 0xF0,  // z
      0x00, 0x00, 0x60, 0x90, 0x90, 0xF0, 0x90, 0x90,  // A
      0x00, 0x00, 0xF0, 0x90, 0xE0, 0x90, 0x90, 0xF0,  // B
      0x00, 0x00, 0x70, 0x88, 0x80, 0x80, 0x88, 0x70,  // C
      0x00, 0x00, 0xE0, 0x90, 0x90, 0x90, 0x90, 0xE0,  // D
      0x00, 0x00, 0xF0, 0x80, 0x80, 0xF0, 0x80, 0xF0,  // E
      0x00, 0x00, 0xF0, 0x80, 0x80, 0xF0, 0x80, 0x80,  // F
      0x00, 0x00, 0x70, 0x88, 0x80, 0x98, 0x88, 0x70,  // G
      0x00, 0x00, 0x90, 0x90, 0x90, 0xF0, 0x90, 0x90,  // H
      0x00, 0x00, 0x70, 0x20, 0x20, 0x20, 0x20, 0x70,  // I
      0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x90, 0xE0,  // J
      0x00, 0x00, 0x90, 0x90, 0xE0, 0x90, 0x90, 0x90,  // K
      0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x90, 0xF0,  // L
      0x00, 0x00, 0x88, 0xD8, 0xA8, 0x88, 0x88, 0x88,  // M
      0x00, 0x00, 0x88, 0x88, 0xC8, 0xA8, 0x98, 0x88,  // N
      0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x88, 0x70,  // O
      0x00, 0x00, 0x70, 0x88, 0x88, 0xF0, 0x80, 0x80,  // P
      0x00, 0x00, 0xF0, 0x90, 0x90, 0x90, 0xF8, 0x08,  // Q
      0x00, 0x00, 0xF0, 0x88, 0x88, 0xF0, 0x90, 0x88,  // R
      0x00, 0x00, 0x70, 0x88, 0xC0, 0x30, 0x88, 0x70,  // S
      0x00, 0x00, 0xF8, 0x20, 0x20, 0x20, 0x20, 0x20,  // T
      0x00, 0x00, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70,  // U
      0x00, 0x00, 0x88, 0x88, 0x88, 0x88, 0x50, 0x20,  // V
      0x00, 0x00, 0x88, 0x88, 0xA8, 0xA8, 0x50, 0x50,  // W
      0x00, 0x00, 0x90, 0x90, 0x60, 0x60, 0x90, 0x90,  // X
      0x00, 0x00, 0x88, 0x88, 0x88, 0x70, 0x20, 0x20,  // Y
      0x00, 0x00, 0xF8, 0x08, 0x30, 0x60, 0xC0, 0xF8,  // Z
      0x00, 0x00, 0x18, 0x18, 0xE0, 0x80, 0x80, 0xE0,  // special character
      0xFC, 0xFF, 0xD5, 0xC5, 0xED, 0xFF, 0xFC, 0x78,  // [])
      0x0, 0xFF, 0x81, 0xC3, 0xA5, 0x99, 0xFF, 0x0,    // []
      0x7E, 0xFF, 0x8F, 0xFF, 0xC3, 0x7E, 0xC, 0x6,   
];
    
var font8x8c = {

    // входные данные для списка символов - должны соответствовать порядку в массиве бинарных данных font8x8 (8 байт на символ)
    textMap : " 1234567890!:.%,+-()?[]'\"=^_абвгдеёжзийклмнопрстуфхцчшщъьыэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЬЫЭЮЯabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
    textMapAdditions : [161, 162, 163, 164], // C0, 
    
    // map : [...] - array of charcodes generated on setFont based on textMap + textMapAdditions arrays
    w : 8,
    h : 8,
    xb : [

        [0, -6, -2],    // space
        [1, -5, -2],    // 1
        [1, -5, -2],    // 2
        [1, -5, -2],    // 3
        [1, -5, -2],    // 4
        [1, -5, -2],    // 5
        [1, -5, -2],    // 6
        [1, -5, -2],    // 7
        [1, -5, -2],    // 8
        [1, -5, -2],    // 9
        [1, -5, -2],    // 0
        [1, -5, -2],    // !
        [1, -7, -2],    // :
        [1, -7, -2],    // .
        [1, -4, -2],    // %
        [0, -6, -1],    // ,
        [1, -6, -2],    // +
        [1, -6, -2],    // -
        [1, -6, -2],    // (
        [1, -6, -2],    // )
        [1, -5, -2],    // ?
        [1, -6, -2],    // [
        [1, -6, -2],    // ]
        [1, -7, -2],    // '
        [1, -6, -2],    // "
        [1, -5, -2],    // =
        [1, -6, -2],    // ^
        [1, -5, -2],    // _
        [1, -4, -2],    // а
        [1, -5, -2],    // б
        [1, -5, -2],    // в
        [1, -5, -2],    // г
        [1, -5, -2],    // д
        [1, -5, -2],    // е
        [1, -4, -2],    // ё
        [1, -3, -2],    // ж
        [1, -5, -2],    // з
        [1, -4, -2],    // и
        [1, -4, -2],    // й
        [1, -4, -2],    // к
        [1, -4, -2],    // л
        [1, -3, -2],    // м
        [1, -4, -2],    // н
        [1, -5, -2],    // о
        [1, -4, -2],    // п
        [1, -5, -2],    // р
        [1, -5, -2],    // с
        [1, -5, -2],    // т
        [1, -5, -1],    // у
        [1, -3, -2],    // ф
        [1, -4, -2],    // х
        [1, -3, -1],    // ц
        [1, -4, -2],    // ч
        [1, -3, -2],    // ш
        [1, -3, -1],    // щ
        [1, -4, -2],    // ъ
        [1, -4, -2],    // ь
        [1, -3, -2],    // ы
        [1, -5, -2],    // э
        [1, -3, -2],    // ю
        [1, -5, -2],    // я
        [1, -4, -2],    // А
        [1, -4, -2],    // Б
        [1, -4, -2],    // В
        [1, -4, -2],    // Г
        [1, -3, -2],    // Д
        [1, -4, -2],    // Е
        [1, -4, -2],    // Ё
        [1, -3, -2],    // Ж
        [1, -4, -2],    // З
        [1, -3, -2],    // И
        [1, -3, -2],    // Й
        [1, -3, -2],    // К
        [1, -3, -2],    // Л
        [1, -3, -2],    // М
        [1, -3, -2],    // Н
        [1, -3, -2],    // О
        [1, -3, -2],    // П
        [1, -3, -2],    // Р
        [1, -3, -2],    // С
        [1, -3, -2],    // Т
        [1, -3, -2],    // У
        [1, -3, -2],    // Ф
        [1, -4, -2],    // Х
        [1, -4, -2],    // Ц
        [1, -4, -2],    // Ч
        [1, -3, -2],    // Ш
        [1, -3, -1],    // Щ
        [1, -3, -2],    // Ъ
        [1, -3, -2],    // Ь
        [1, -3, -2],    // Ы
        [1, -4, -2],    // Э
        [1, -3, -2],    // Ю
        [1, -4, -2],    // Я
        [1, -4, -2],    // a
        [1, -5, -2],    // b
        [1, -5, -2],    // c
        [1, -5, -2],    // d
        [1, -5, -2],    // e
        [1, -5, -2],    // f
        [1, -5, -2],    // g
        [1, -5, -2],    // h
        [0, -6, -2],    // i
        [1, -6, -1],    // j
        [1, -5, -2],    // k
        [0, -6, -2],    // l
        [1, -3, -2],    // m
        [1, -4, -2],    // n
        [1, -5, -2],    // o
        [1, -5, -2],    // p
        [1, -5, -2],    // q
        [1, -4, -2],    // r
        [1, -5, -2],    // s
        [1, -5, -2],    // t
        [1, -4, -2],    // u
        [1, -4, -2],    // v
        [1, -3, -2],    // w
        [1, -4, -2],    // x
        [1, -5, -1],    // y
        [1, -3, -2],    // z
        [1, -4, -2],    // A
        [1, -4, -2],    // B
        [1, -3, -2],    // C
        [1, -4, -2],    // D
        [1, -4, -2],    // E
        [1, -4, -2],    // F
        [1, -3, -2],    // G
        [1, -3, -2],    // H
        [0, -4, -2],    // I
        [1, -3, -2],    // J
        [1, -3, -2],    // K
        [1, -4, -2],    // L
        [1, -3, -2],    // M
        [1, -3, -2],    // N
        [1, -3, -2],    // O
        [1, -3, -2],    // P
        [1, -3, -1],    // Q
        [1, -3, -2],    // R
        [1, -3, -2],    // S
        [1, -3, -2],    // T
        [1, -3, -2],    // U
        [1, -3, -2],    // V
        [1, -3, -2],    // W
        [1, -4, -2],    // X
        [1, -3, -2],    // Y
        [1, -3, -2],    // Z
        [0, -3, -2, 'celsius degree'],   
        [0, 0, 0, 'icon - tea cup with heart'],   
        [0, 0, 0, 'icon - letter'], 
        [0, 0, 0, 'icon - message bubble'],   
    ]
};


/*
    String.fromCharCode(0x33)
    var map = "0123456789.: ates";
    var codes = [];
    for (var i = 0; i < map.length; i++) {

        codes.push(map[i].charCodeAt(0));
    }

    Object.keys(codes).forEach(e => codes[e] = "0x" + codes[e].toString(16));
    
    
    
    var charCodeToByte = 999;
    console.log('0x' + charCodeToByte.toString(16));
*/

// оптимизировать вывод - вычислять ширину через отдельную функцию и сразу скипать циклы x y если выходим за границы отрисовки

var screen = new Object();
    screen.el = document.getElementById('screen');
    // screen.width = 64;
    // screen.height = 24;
    screen.width = 32;
    screen.height = 20;
    
    screen.player = {state : "idle"};
    screen.buffer = [];
    screen.renderFrame = false;
    screen.flips = [];
    
    screen.getWidth = function() {
        return screen.width;
    };
    
    screen.getHeight = function() {
        return screen.height;
    };
    
    screen.init = function() {
            
            var table = document.createElement('table');
            screen.el.appendChild(table);
            
            var tableHTML = '';
            for (var y = 1; y <= screen.getHeight(); y++) {
                
                screen.buffer[y] = [];
                tableHTML += '<tr>';
                
                for (var x = 1; x <= screen.getWidth(); x++) {
                    tableHTML += '<td id="screen-' + x + '-' + y + '"></td>';
                    screen.buffer[y][x] = 0;
                }
                
                tableHTML += '</tr>';
            }
            
            table.innerHTML = tableHTML;
            
            var hideForms = function() {
                var forms = document.getElementsByClassName('form-data');
                for (var i = 0; i < forms.length; i++) {
                    forms[i].style.display = 'none';
                }                
            }
            
            var utf8Encoder = new TextEncoder();
            var utf8ToHexNotice = function(s) {
            
                  const rb = utf8Encoder.encode(s);
                  let r = '';
                  
                  for (const b of rb) {
                    r += 'tt' + '.append(1, 0x' + ('0' + b.toString(16)).slice(-2) + '); ';
                  }
                  
                  return r;
            }

            var getGlyphNotice = function(index) {                
                
                var charCode = screen.fontSettings.map[index];
                
                var charI = String.fromCharCode(charCode);
                if (charI == ' ') charI = 'space';
                
                if (screen.fontSettings.textMapAdditions.indexOf(charCode) != -1 ) {
                    
                    var title = "special character";
                    
                    if (typeof screen.fontSettings.xb[index][3] != 'undefined') {
                        title = screen.fontSettings.xb[index][3];
                    }
                    
                    charI = String.fromCharCode(charCode) + ' | ' + title + ' | UTF8 code : ' + charCode + ' | HEX : ' + utf8ToHexNotice(String.fromCharCode(charCode));
                }
                
                return charI;
            }
                
                
            var formatHex = function(c) {
            
                var h = c.toString(16);
                if (h.length < 2) {
                    h = '0' + h;
                }
                
                h = '0x' + h.toUpperCase();
                return h;
            }
            
            document.getElementById('test-audio').onclick = function() {
                
                if (screen.player.state == "play") {
                    
                    screen.player.started = false;
                    screen.player.src.stop();
                    screen.player.state = "idle";
                    
                    clearInterval(screen.player.interval);
                    
                    screen.player.updateBtnState();
                    
                    return;
                    
                } else if (screen.player.state == "load") {                        
                    
                    return;
                }
                
                var song = "Portal OST - Radio.mp3";
                
                // https://webref.ru/dev/canvasdeepdive/chapter09
                screen.reset();
                if (!screen.tScroll) screen.tScroll = 9;
                screen.tScrolled = 0;
                
                screen.drawString(1, 2, "loading...");  
                screen.drawStringScrolled(screen.tScroll, 12, song, 4);
                           
                var player = {};
                    
                    player.ctx; // контекст аудио
                    player.buf = false; // аудиобуфер
                    player.fft; // узлы аудио 
                    player.samples = 128; 
                    player.btn = this;
                    player.state = "load";
                  
                    player.init = function() { 
                        window.AudioContext = window.AudioContext || window.webkitAudioContext;
                        console.log("in init"); 
                        player.ctx = new window.AudioContext(); //is there a better API for this? 
                    } 
                    
                    player.loadFile = function() { 
                    
                      var req = new XMLHttpRequest(); 
                          req.open("GET", song, true); 
                          
                          req.responseType = "arraybuffer"; 
                          req.onload = function() {
                              
                                for (var y = 1; y <= 11; y++) {
                                    
                                    for (var x = 1; x <= screen.getWidth(); x++) {
                                        screen.buffer[y][x] = 0;
                                    }
                                }
                                
                                screen.update();
                                
                                screen.drawString(2, 2, "play"); 
                                
                                // декодируем загруженные данные
                                
                                player.ctx.decodeAudioData(req.response, function(buffer) { 
                                    player.buf = buffer; 
                                    player.play(); 
                                }); 
                          }; 
                          
                          req.send(); 
                    }
                    
                    player.formatSeconds = function(s) {
                        var date = new Date(null);
                        date.setSeconds(s); 
                        var result = date.toISOString().slice(11, 19);
                        
                        return result;
                    }
                    
                    player.updateBtnState = function() {
                        
                        var text = "Test audio";
                                                     
                        if (player.state == "play") {
                           text = player.formatSeconds(screen.player.ctx.currentTime) + " / " + player.formatSeconds(player.duration);
                        }
                        
                        player.btn.innerText = text;
                        
                    }
                    
                    player.play = function() { 
                          // создаём исходный узел из буфера
                          player.src = player.ctx.createBufferSource(); 
                          player.src.buffer = player.buf; 
                          
                          // создаём fft 
                          player.fft = player.ctx.createAnalyser(); 
                          player.fft.fftSize = player.samples; 
                          
                          // соединяем в цепочку
                          player.src.connect(player.fft); 
                          player.fft.connect(player.ctx.destination); 
                          
                          // сразу воспроизводим
                          // player.src.noteOn(0); 
                          player.src.start();
                          player.src.loop = true;
                          
                          player.state = "play"; 
                          
                          player.interval = setInterval(player.updateScreen, 100);                          
                          player.duration = screen.player.src.buffer.duration;
                          
                          player.updateScreen();
                    }
                    
                    player.updateScreen = function() { 
                
                            for (var y = 1; y <= 11; y++) {
                                
                                for (var x = 1; x <= screen.getWidth(); x++) {
                                    screen.buffer[y][x] = 0;
                                }
                            }
                            
                                
                          //var data = new Uint8Array(player.samples); 
                          //player.fft.getByteFrequencyData(data); 
                          
                          var waveform = new Float32Array(player.fft.frequencyBinCount);                          
                          player.fft.getFloatTimeDomainData(waveform);
                            
                          var pt = 1 / 100;
                          var pixelsY = 10;
                          var ptScreen = pixelsY / 100;
                          
                          var length = waveform.length;
                          var pixelsX = 32;
                          var step = Math.round(length / pixelsX);
                          var x = 1;
                          
                          for(var i=0; i < length; i+=step) { 
                                
                                var val = (waveform[i] / 2) + 0.5;
                                if (val < 0) val = 0;
                                
                                var percent = (val) / pt;
                                if (percent > 100) percent = 100;
                                
                                var yMax = Math.round(ptScreen * percent);
                               
                                if (yMax >= 1) {
                                    screen.buffer[yMax][x] = 1;
                                }
                                
                                x++;
                          } 
                          
                          
                          window.tttwaveform = waveform;
                          
                            
                         screen.update();
                         player.updateBtnState();
                         // requestAnimationFrame(player.updateScreen);
                    }
                    
                    player.init();
                    player.loadFile();
                    
                    screen.player = player;
            }
            
            document.getElementById('test-scroll').onclick = function() {
                
                if (screen.player.state != "idle") return;
                
                hideForms();
                screen.reset();
                if (!screen.tScroll) screen.tScroll = 9;
                screen.tScrolled = 0;
                
                var textData = document.getElementById('text-data');
                    textData.style.display = "";
                    textData.onchange = function() {
                        screen.reset();
                        screen.drawStringScrolled(screen.tScroll, 4, textData.value, 4);
                    }
                    
                screen.drawStringScrolled(screen.tScroll, 4, textData.value, 4);
            }
            
            document.getElementById('image-to-src').onclick = function() {
                
                hideForms();
                var form = document.getElementById('image-to-src-form');
                    form.style.display = '';
                    
                var run = document.getElementById('image-to-src-run');
                    run.onclick = function() {
                    
                                    
                        var file = document.getElementById('image-to-src-file');
                        if (file.files.length <= 0) {
                            document.getElementById('image-to-src-error').innerText = 'Файл не выбран';
                            return;
                        }
                                   
                        var reader = new FileReader();
                            reader.addEventListener("loadend", function(arg) {
                            
                                var view = new Uint8Array( this.result );
                                var kb = view.length / 1000;
                                
                                if (kb > 100) {
                                    document.getElementById('image-to-src-error').innerText = 'Слишком большой файл - ' + kb + 'кб';
                                    return;
                                }
                                
  
  

                                var header = '';
                                    header += '/*' +'\r\n';
                                    header += 'typedef struct {' +'\r\n';
                                    header += '           byte * data;' +'\r\n';
                                    header += '           String type;' +'\r\n';
                                    header += '           int size;' +'\r\n';
                                    header += '} webData;' +'\r\n';
                                    header += '*/' +'\r\n';
                                    header += '\r\n';
                                    header += '\r\n';
                                    header += '\r\n';
                                    header += '\r\n';
                                    header += '\r\n';
                                    
                                var txt = '';    
                                    
                                for ( var i = 0; i < view.length; i++) {
                                    var hex = view[i].toString(16); 
                                    if (hex == '0') hex += '0';
                                    txt += (txt ? ',' : '') + '0x' + hex;
                                }
                                
                                var resultTxt = '';
                                    resultTxt += 'webData file1;' + '\r\n';
                                    resultTxt += '        file1.size = ' + view.length + ';' + '\r\n';
                                    resultTxt += '        file1.type = "' + file.files[0].type + '";' + '\r\n';
                                    resultTxt += '        file1.data = {' + txt + '};' + '\r\n';
                                
                                
                                var result = document.getElementById('image-to-src-result');
                                    result.style.display = '';
                                    result.value = header + resultTxt;
                                
                            });
                            
                            reader.readAsArrayBuffer(file.files[0]);
                    }
            }
            
            document.getElementById('test-clock').onclick = function() {
                
                if (screen.player.state != "idle") return;
                
                hideForms();
                screen.reset();
                screen.showTime();
            }
            
            document.getElementById('show-font-src').onclick = function() {
            
                hideForms();
                var textBlock = document.getElementById('font-src');   
                
                var text = '';
                    text += '/* total glyphs : ' + screen.fontSettings.map.length + '*/' + '\r\n';
                    text +=  '\r\n' +  '\r\n' +  '\r\n';
                    
                var glyphSettingsNum = 3;
                
                var fontName = 'font' + screen.fontSettings.w + 'x' + screen.fontSettings.h;
                var fontMapName = fontName + 'Map';
                var fontGlyphSettingsName = fontName + 'GlyphSettings';
                var fontManifestName = fontName + 'Config';
                
                /* glyphs binary */
                
                var bytesPerItem = Math.ceil((screen.fontSettings.h * screen.fontSettings.w) / 8);
                var currentRow = 0;
                var mapIndex = 0;
                
                
                text += 'const static int ' + fontName + '[] PROGMEM  = {' +  '\r\n';
                text += '        ';
                
                for (var i = 0; i < screen.font.length; i++) {
                    
                    text += formatHex(screen.font[i]) + ',';
                    currentRow++;
                    
                    if (currentRow == bytesPerItem) {
                        currentRow = 0;
                        text += '        // '  + getGlyphNotice(mapIndex) + '\r\n';
                        if (i < screen.font.length-1) {
                            text += '        ';
                        }
                        
                        mapIndex++;
                    }
                }
                
                text += '};' +  '\r\n';
                text +=  '\r\n' +  '\r\n' +  '\r\n';
                
                /* glyphs settings */
                
                var codeMap = '';
  
                for (var i = 0; i < screen.fontSettings.map.length; i++) {
                    codeMap += screen.fontSettings.map[i] + ',';
                }
                
                text +=  '\r\n' +  '\r\n' +  '\r\n';
                
                text += 'const static int ' + fontName + 'Map[] PROGMEM = {' + '\r\n' + codeMap +  '\r\n' + '};' +  '\r\n';
                
                text +=  '\r\n' +  '\r\n' +  '\r\n';
                
                text += 'const glyphSettings ' + fontName + 'GlyphSettings[] PROGMEM = {' + '\r\n';
                
                for (var i = 0; i < screen.fontSettings.xb.length; i++) {
                
                    var glyphSettings = screen.fontSettings.xb[i];                    
                    text += '        {';
                    var settingNoticePading = '';
                    
                    for (var b = 0; b < glyphSettingsNum; b++) {
                        
                        var settingValue = typeof glyphSettings[b] == 'undefined' ? '0' : glyphSettings[b].toString();
                        if (settingValue.length <= 1) settingNoticePading += ' ';
                        
                        text += settingValue;
                        
                        if (b < glyphSettingsNum-1) {
                            text += ', ';
                        }
                    }   

                    text += '}, ' + settingNoticePading + '// ' + getGlyphNotice(i);       
                    text +=  '\r\n';                
                }
                
                text +=  '};' + '\r\n';
                text +=  '\r\n' +  '\r\n' +  '\r\n';
                
                text += 'const fontManifest ' + fontManifestName + ' PROGMEM = {' + '\r\n';
                text += '   (int *) ' + fontName + ',' + '\r\n';
                text += '   (int *) ' + fontMapName + ',' + '\r\n';
                text += '    ' + screen.fontSettings.w + ',' + '\r\n';
                text += '    ' + screen.fontSettings.h + ',' + '\r\n';
                text += '    sizeof(' + fontMapName + ') / sizeof(' + fontMapName + '[0])' + ',' + '\r\n';
                text += '   (glyphSettings *) ' + fontGlyphSettingsName + '' + ',' + '\r\n';
                text += '};';
                
                textBlock.value = text;
                textBlock.style.display = '';
            };
            
    };
    
    screen.clear = function() {
        
         for (var y = 1; y <= screen.getHeight(); y++) {
                
                for (var x = 1; x <= screen.getWidth(); x++) {
                    screen.buffer[y][x] = 0;
                }
            }
            
    };
           
    screen.setPixel = function(x, y, color) {
         
          if (screen.renderFrame) {
              if (y > screen.renderFrame.y + screen.renderFrame.height - 1) {
                return;
              } else if (y < screen.renderFrame.y) {
                return;
              }

              if (x > screen.renderFrame.x + screen.renderFrame.width - 1) {
                return;
              } else if (x < screen.renderFrame.x) {
                return;
              } 
        }
        
        if (typeof screen.buffer[y] != 'undefined' && typeof screen.buffer[y][x] != 'undefined') {
            screen.buffer[y][x] = color;            
        }
        
    };
    
    screen.getGlyphByChar = function(c) {
        return screen.fontSettings.map.indexOf(c.charCodeAt(0));
    };
    
    screen.setFont = function(fontData, fontSettings) {
    
        screen.font = fontData;
        screen.fontSettings = fontSettings;
        
               // String.fromCharCode(0x33)
     
        screen.fontSettings.map = [];
        for (var i = 0; i < screen.fontSettings.textMap.length; i++) {
            screen.fontSettings.map.push(screen.fontSettings.textMap[i].charCodeAt(0));
        }
        
        for (var i = 0; i < screen.fontSettings.textMapAdditions.length; i++) {
            screen.fontSettings.map.push(screen.fontSettings.textMapAdditions[i]);
        }
    };
    
    screen.getBit = function(byte, position) // position in range 0-7
    {
        return (byte >> (7 - position)) & 0x1;
    };
    
    screen.drawGlyph = function(screenX, screenY, c) {
    
        var charIndex = screen.getGlyphByChar(c);
        if (charIndex == -1) {
            console.log('no symbol ' + c);
            return false;
        }
        
        var bit = Math.ceil((screen.fontSettings.h * screen.fontSettings.w) / 8) * charIndex, bitCursor = 0;
        
        // console.log(screenX + ' | ' + screenY + ' |||' + c);
          
        for (var glyphY = 1; glyphY <= screen.fontSettings.h; glyphY++) {
               
              var y = screenY + glyphY - 1;
              
              if (typeof screen.fontSettings.xb[charIndex][2] != 'undefined') y += screen.fontSettings.xb[charIndex][2];
              
              for (var glyphX = 1; glyphX <= screen.fontSettings.w; glyphX++) {
            
                    var glyphXYpixel = screen.getBit(screen.font[bit], bitCursor) ? true : false;  
                    if (glyphXYpixel) screen.setPixel(screenX + glyphX - 1, y, glyphXYpixel);
               
                    bitCursor++;
                    
                    if (bitCursor > 7) {
                        bit++;
                        bitCursor = 0;                            
                    }                   
              }
              
        }
    };
    
    screen.drawString = function(screenX, screenY, text, fillXpadding) {
        // console.log(1);
        
        var posX = screenX;
        screen.posMap = [];
        
        
        for ( var i = 0; i < text.length; i++) {
           
                var index = screen.getGlyphByChar(text[i]);
                if (index === -1) {
                    console.error('UNKNOWN CHAR : '  + text[i]);
                    continue;
                }
                
             if (i > 0) posX += screen.fontSettings.xb[index][0];
             screen.posMap.push(posX);
             
             screen.drawGlyph(posX, screenY, text[i]);
             
             posX += screen.fontSettings.w + screen.fontSettings.xb[index][1];
            
        }
        
        var width = posX-screenX;
        if (width <= 0) width = 1;
        
        var iter = 0;
        
        if (fillXpadding) {
            
            //  && posX < screen.width
            
            posX += fillXpadding;
            
            // if (screenX-fillXpadding > 1) {
            //    screen.drawString(screenX - width - fillXpadding, screenY, text);
            //}
            
            var leftPosX = screenX - width - fillXpadding;
            
            while (leftPosX + width >= 1 && iter < 100) {
                iter++;
                screen.drawString(leftPosX, screenY, text);
                leftPosX -= width;
                leftPosX -= fillXpadding;             
            }
            
            if (iter >= 100) debugger;
            
            iter = 0;
            while (posX < screen.width  && iter < 100) {
                iter++;
                screen.drawString(posX, screenY, text);
                posX += width + fillXpadding;                
            }
            
            //width += screen.drawString(posX + fillXpadding, screenY, text);
            //width += screen.drawString(posX + fillXpadding + width, screenY, text);
        }
        
        return width;
    };
    
    screen.transitionString = function(x, y, from, to, scrollCurrent, renderFrame) {
    
        if (typeof scrollCurrent == 'undefined') scrollCurrent = 0;
        
        screen.renderFrame = renderFrame;
        
        for (var frameY = screen.renderFrame.y; frameY <= screen.renderFrame.y + screen.renderFrame.height; frameY++) {
        
              for (var frameX = screen.renderFrame.x; frameX <= screen.renderFrame.x + screen.renderFrame.width; frameX++) {                   
                    screen.buffer[frameY][frameX] = false;
                    console.log(y + '  ' + x);
               }                          
        }
        
        screen.drawString(x, y + scrollCurrent, from);
        screen.drawString(x, y - screen.fontSettings.h - 1 + scrollCurrent, to);
        
        if (scrollCurrent <= screen.fontSettings.h) {
            setTimeout(function() { screen.transitionString(x, y, from, to, scrollCurrent+1, renderFrame); }, 100);
        } else screen.renderFrame = false;
        
        screen.update();
    };

    screen.drawStringScrolled = function(scrollX, scrollY, text, paddingX) {
        
        if (!paddingX) paddingX = 4;
        
        screen.drawString(scrollX, scrollY, text, paddingX);
        
        screen.tScrollEnabled = true;
        screen.tScrolledBaseX = scrollX;
        screen.tScrolledCurrentX = scrollX;
        screen.tScrolledInterval = setInterval( function() {  
        
            for (var frameY = scrollY; frameY <= scrollY + screen.fontSettings.h; frameY++) {
            
                  for (var frameX = 1; frameX <= screen.getWidth(); frameX++) {                   
                        screen.buffer[frameY][frameX] = false;
                   }                          
            }
            
            
            var paddingX = 4;
            var width = screen.drawString(screen.tScrolledCurrentX, scrollY, text, paddingX);  
            screen.tScrolledCurrentX--;
            
            if (Math.abs(screen.tScrolledBaseX - screen.tScrolledCurrentX) >= width + paddingX) {
                screen.tScrolledCurrentX = screen.tScrolledBaseX;
            }
            
            screen.update();
        }, 100);
    };
    
    screen.updateTime = function() {
    
            if (!screen.date) screen.date = 1692571560012;
            
            var screenX = 1;
            var screenY = 4;
            
            // "physical" bounds of pixels inside of digits glyph bitmap, to fit in monotype \ bounds of clock
            var glyphWidth = 3;
            var glyphHeight = 6;
            
            var clearXoffset = (screen.fontSettings.w - glyphWidth) * -1;
            var clearYoffset = (screen.fontSettings.h - glyphHeight) * -1;
            var localHeight = screen.fontSettings.h + clearYoffset;
            
            
            console.log(localHeight);
            console.log(clearYoffset + '  ' + clearXoffset);
            
            screen.date+=1000;
            
            var date = new Date(screen.date);
            
            var hours = date.getHours();
            var minutes = date.getMinutes();
            var seconds = date.getSeconds();
            
            if (hours < 10){
                hours = "0" + hours;
            }
            
            if (minutes < 10){
                minutes = "0" + minutes;
            }
            
            if (seconds < 10){
                seconds = "0" + seconds;
            }
            
            screen.time2 = hours + ':' + minutes + ':' + seconds;
            console.log(screen.time2);
            
            var timeNow = hours + minutes + seconds;
            
            var flipDigit = function(digit) {
            
                  var flipChar = {
                    
                        from : screen.timeBefore[digit],
                        to : timeNow[digit],
                        
                        scrollY : 1,
                        screenX : screen.timePos[digit],
                        screenY : screenY,
                        tm : false,
                        
                        draw : function() {
                        
                            screen.renderFrame = {x : screenX, y : screenY - 1, width : screen.getWidth(), height : localHeight + 2}; // check with screenY - padding
                            
                            for (var frameY = flipChar.screenY-1; frameY <= flipChar.screenY + screen.renderFrame.height; frameY++) {
                            
                                  for (var frameX = flipChar.screenX; frameX <= flipChar.screenX + screen.fontSettings.w + clearXoffset; frameX++) {
                                       
                                        screen.setPixel(frameX, frameY, false);
                                  }                          
                            }
                            
                            screen.drawGlyph(flipChar.screenX, flipChar.screenY - localHeight - 1 + flipChar.scrollY, flipChar.to); 
                            screen.drawGlyph(flipChar.screenX, flipChar.screenY + flipChar.scrollY, flipChar.from);
                               
                            screen.renderFrame = false;
                            
                        },
                        
                        animate : function() {
                            
                            // console.log('frame ' + flipChar.scrollY);
                            flipChar.draw();
                            
                            screen.update();
                            
                            if (flipChar.scrollY < localHeight + 1) {
                                tm = setTimeout(flipChar.animate, 100);
                            } else flipChar.end = true;
                            
                            flipChar.scrollY++;
                        },
                        
                        stop : function() {
                            flipChar.end = true;
                            clearTimeout(tm);
                        }
                    };
                    
                    flipChar.animate();
                    return flipChar;
            };
            
            
            if (typeof screen.timeBefore == 'undefined') {
      
                screen.timeBefore = '133734'; 
                screen.drawString(screenX, screenY, screen.time2);
                screen.timePos = [screen.posMap[0], screen.posMap[1], screen.posMap[3], screen.posMap[4], screen.posMap[6], screen.posMap[7]];
            }

            for (var digit = 0; digit <= 5; digit++) {

                if (timeNow[digit] == screen.timeBefore[digit]) continue;
                // if (typeof screen.flips[digit] != 'undefined') {
                //    screen.flips[digit].stop();
                //}
                
                screen.flips[digit] = flipDigit(digit);
            }
            
            screen.timeBefore = hours + minutes + seconds;
            screen.update();
    };
    
    screen.reset = function() {
        
         screen.clear();
         
        
        clearInterval(screen.tScrolledInterval);
        clearInterval(screen.timeInterval);
        
        for (var digit = 0; digit <= 5; digit++) {

            if (typeof screen.flips[digit] != 'undefined') {
                screen.flips[digit].stop();
            }
        }
        
        screen.timeEnabled = false;
        screen.tScrollEnabled = false;
    }
    
    screen.showTime = function() {
    
         screen.reset();
         
         delete screen.timeBefore;
         
         screen.updateTime();
         screen.timeEnabled = true;
         screen.timeInterval = setInterval(screen.updateTime, 1000);   
         
         
         screen.drawString(1, 12, "24.1 " + String.fromCharCode(161));        
          
         screen.drawString(screen.getWidth()-3, screen.getHeight()-6, '.');
        
    }
    
    screen.drawMain = function(text) {
        
        // screen.setFont(font5x6, font5x6c);
         screen.setFont(font8x8, font8x8c);
        /*screen.setFont(font12x16, font12x16c);*/
        
        
        screen.drawStringScrolled(1, 4, '22.1' + String.fromCharCode(161) + ' ' + String.fromCharCode(163) + ' ' + String.fromCharCode(164) + ' test', 4);

        /* screen.drawString(1, 4, '22:56:32');  */
        
        
        //screen.showTime();
        
        // setTimeout(function() {
        // todo - check transition string + transition bitmap from left
        //    screen.transitionString(1, 4, ' тест', '12:56:32', 0, {x : 1, y : 3, width : 32, height : 6});
        //
        // }, 500);
        
      
        
        /*
        setTimeout(function() {
        
            console.log('draw');
            console.log(screen.posMap[screen.posMap.length-1]);
            
            // screen.drawGlyph(screen.posMap[screen.posMap.length-1], screenY, "9"); 
            
            var flipChar = {
            
                from : "2",
                to : "9",
                
                scrollY : 1,
                
                screenX : screen.posMap[screen.posMap.length-1],
                screenY : 4,
                
                draw : function() {
                
                    for (var frameY = flipChar.screenY-1; frameY <= flipChar.screenY + screen.fontSettings.h; frameY++) {
                    
                          for (var frameX = flipChar.screenX; frameX <= flipChar.screenX + screen.fontSettings.w; frameX++) {
                               
                                screen.setPixel(frameX, frameY, false);
                          }                          
                    }
                   //  screen.update();
                   // debugger;
                    screen.drawGlyph(flipChar.screenX, flipChar.screenY + flipChar.scrollY, flipChar.from, false, flipChar.screenY, false, screen.fontSettings.h);
                    screen.drawGlyph(flipChar.screenX, flipChar.screenY - screen.fontSettings.h - 1 + flipChar.scrollY, flipChar.to, false, flipChar.screenY-1, false, screen.fontSettings.h+1);
            
                },
                
                animate : function() {
                    
                    console.log('frame ' + flipChar.scrollY);
                    flipChar.draw();
                    
                    screen.update();
                    
                    if (flipChar.scrollY < screen.fontSettings.h + 1) {
                        setTimeout(flipChar.animate, 100);
                    } else flipChar.end = true;
                    
                    flipChar.scrollY++;
                }
            }
            
            flipChar.animate();
            
        }, 2000);
        
        */
        screen.update();
    };
    
    screen.update = function() {
        
         for (var y = 1; y <= screen.getHeight(); y++) {
                
                for (var x = 1; x <= screen.getWidth(); x++) {
                    var pixel = document.getElementById('screen-' + x + '-' + y);
                    if (screen.buffer[y][x]) {
                        pixel.classList.add('active');
                    } else {
                        pixel.classList.remove('active');
                    }
                }
            }
        
    };
    
    
    
    screen.init();
    screen.drawMain();

</script>
</body>
</html>