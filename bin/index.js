let FieldMessage = require('./protos/life_field_pb');

let canvas = document.getElementById("canvas");
let ctx = canvas.getContext('2d');
let next_btn = document.getElementById("next");
let start_btn = document.getElementById("start");
let stop_btn = document.getElementById("stop");
let preset_selector = document.getElementById("preset");
let load_btn = document.getElementById("load");

let width = 60;
let height = 60;
let intervalBetweenFrames = 1;

let field = [];

let is_active = false;

class Cell {
    static DEAD = 0;
    static ALIVE = 1;
    static RECT_WIDTH = 10;
    static RECT_HEGIHT = 10;
    static toggleCell (cell) {
        return cell == Cell.DEAD ? Cell.ALIVE : Cell.DEAD;
    }
};

for (let i = 0; i < height; ++i) {
    field[i] = [];
    for (let j = 0; j < width; ++j) {
        field[i][j] = Cell.DEAD;
    }
}

canvas.addEventListener("click", function(e) {
    if (is_active) {
        return;
    }

    const rect = canvas.getBoundingClientRect();
    const x = e.clientX - rect.left;
    const y = e.clientY - rect.top;

    let col = Math.floor(x / Cell.RECT_WIDTH);
    let row = Math.floor(y / Cell.RECT_HEGIHT);

    if (row < height && col < width) {
        field[row][col] = Cell.toggleCell(field[row][col]);
    }

    render();
});

function render() {
    for (let i = 0; i < height; ++i) {
        for (let j = 0; j < width; ++j) {
            let color = 'black';
            if (field[i][j] == Cell.ALIVE) {
                color = 'white';
            }
            ctx.fillStyle = color;
            ctx.fillRect(j * Cell.RECT_WIDTH, i * Cell.RECT_HEGIHT, Cell.RECT_WIDTH, Cell.RECT_HEGIHT);
        }
    }
    ctx.fill();
    ctx.stroke();
}

render();
is_active = false;

function requestPreset(preset) {
    return fetch('/preset/' + preset);
}

function requestNextStep() {
    let protoField =  new FieldMessage.Field();
    protoField.setWidth(width);
    protoField.setHeight(height);
    protoField.setRaw(
        field.map(a => a.join('')).join('')
    );
    return fetch('/next', {
        method: 'POST',
        body: protoField.serializeBinary()
    })
}

function renderFromRaw(text) {
    let [newWidth, newHeight, newField] = text.split(' ');
    width = newWidth;
    height = newHeight;
    for (let i = 0; i < height; ++i) {
        field[i] = [];
        for (let j = 0; j < width; ++j) {
            field[i][j] = newField[i * width + j] == '0' ? Cell.DEAD : Cell.ALIVE;
        }
    }
    render();
}

next_btn.addEventListener('click', function() {
    requestNextStep().then(
        response => response.text()
    ).then(renderFromRaw);
});

let nextStepAnimator;

start_btn.addEventListener('click', function() {
    is_active = true;
    (function onStartBtnClick() {
        if (is_active) {
            requestNextStep().then(
                response => response.text()
            ).then(
                text => {
                    renderFromRaw(text);
                    nextStepAnimator = setTimeout(onStartBtnClick, intervalBetweenFrames);
                }
            );
        }
    })();
});

stop_btn.addEventListener('click', function() {
    is_active = false;
    clearTimeout(nextStepAnimator);
});

load_btn.addEventListener('click', function() {
    requestPreset(preset_selector.value)
    .then(
        response => response.json()
    )
    .then(
        json => {
            let newField = json.field;
            width = json.width;
            height = json.height;
            for (let i = 0; i < height; ++i) {
                field[i] = [];
                for (let j = 0; j < width; ++j) {
                    field[i][j] = newField[i][j] == '0' ? Cell.DEAD : Cell.ALIVE;
                }
            }
            render();
        }
    )
});
