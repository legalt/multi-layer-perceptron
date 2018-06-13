(function () {
    let $canvas = window.Paint,
        paint = false,                
        
        clickX = [],
        clickY = [],
        clickDrag = [],
        
        
        addClick = function ( x, y, dragging ) {
            clickX.push(x);
            clickY.push(y);
            clickDrag.push(dragging);
        },


        press = function ( event ) {
            let mouseX = event.pageX - this.offsetLeft,
                mouseY = event.pageY - this.offsetTop;
    
            paint = true;
            
            context.closePath();
            context.beginPath();

            context.fillStyle = context.strokeStyle;
            context.fillRect(0, 0, paint.width, paint.height);

            addClick(event.pageX - this.offsetLeft, event.pageY - this.offsetTop);
            redraw();
        },


        drag = function ( event ) {
            let mouseX = event.pageX - this.offsetLeft,
                mouseY = event.pageY - this.offsetTop;
            
            if ( paint ) {
                addClick(mouseX, mouseY, true);
                redraw();
            }

            event.preventDefault();
        },

        release = function ( event ) {
            paint = false;
            redraw();
        },

        cancel = function ( event ) {
            paint = false;
        },
        
        
        context = $canvas.getContext("2d");

    $canvas.addEventListener('mousedown', press, false);
    $canvas.addEventListener('mousemove', drag, false);
    $canvas.addEventListener('mouseup', release);
    $canvas.addEventListener('mouseleave', cancel);            

    function redraw ( ) {
        context.clearRect(0, 0, context.canvas.width, context.canvas.height); // Clears the canvas
        
        context.strokeStyle = "#df4b26";
        context.lineJoin = "round";
        context.lineWidth = 20;
        
        for ( let index = 0; index < clickX.length; index++ ) {
            context.beginPath();

            if( clickDrag[index] && index ) {
                context.moveTo(clickX[index - 1], clickY[index - 1]);
            } else {
                context.moveTo(clickX[index] - 1, clickY[index]);
            }
            
            context.lineTo(clickX[index], clickY[index]);
            context.closePath();
            context.stroke();
        }
    }


    window.recognize.addEventListener('click', function () {
        let img = new Image(28, 28);
        
        img.onload = function () {
            context.clearRect(0, 0, 250, 250);
            
            context.drawImage(img, 0, 0, 250, 250, 0, 0, 28, 28);

            let imgData = context.getImageData(0, 0, 28, 28),
                pixels = imgData.data,
                xhr = new XMLHttpRequest(),
                newPixels = [];
            
            context.clearRect(0, 0, 250, 250);
                
            for ( let index = 0; index < pixels.length; index += 4 ) {
                let pixel = 0.34 * pixels[index] + 0.5 * pixels[index + 1] + 0.16 * pixels[index + 2];
                
                newPixels.push(pixel / 255);
            }
            
            xhr.open('POST', 'http://10.110.22.68:8080', true);

            xhr.onreadystatechange = function () {
                if ( this.readyState !== 4 ) return;

                let result = JSON.parse(this.responseText),
                    item = Math.max.apply(null, result),
                    index = result.indexOf(item);
                
                window.res.value = index === -1 ? "Not recognized" : index;
            };

            xhr.send(newPixels);
        }

        img.src = $canvas.toDataURL();
        clickX = [];
        clickY = [];
        clickDrag = [];
    });
})();