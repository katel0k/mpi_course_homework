let canvas = document.getElementById("canvas");
let ctx = canvas.getContext('2d');
let next_btn = document.getElementById("next");
let prev_btn = document.getElementById("prev");
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
    if (!is_active) {
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
is_active = true;

function requestPreset(preset) {
    return fetch('/preset/' + preset);
}

function requestNextStep() {
    return fetch('/next', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json;charset=utf-8'
        },
        body: JSON.stringify({
            field, width, height
        })
    })
}

// function requestPrevStep() {
//     return fetch('/prev', {
//         method: 'POST',
//         headers: {
//             'Content-Type': 'application/json;charset=utf-8'
//         },
//         body: JSON.stringify({
//             field
//         })
//     })
// }

next_btn.addEventListener('click', function() {
    requestNextStep().then(
        response => response.json()
    ).then(
        json => {
            field = JSON.parse(json).field;
            render();
        }
    );
});

let nextStepAnimator;

start_btn.addEventListener('click', function onStartBtnClick() {
    requestNextStep().then(
        response => response.json()
    ).then(
        json => {
            field = JSON.parse(json).field;
            render();
            nextStepAnimator = setTimeout(onStartBtnClick, intervalBetweenFrames);
        }
    );
});

stop_btn.addEventListener('click', function() {
    clearTimeout(nextStepAnimator);
});

// prev_btn.addEventListener('click', function() {
//     requestNextStep().then(
//         response => response.json()
//     ).then(
//         json => {
//             field = JSON.parse(json).field;
//             render();
//         }
//     );
// });

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
