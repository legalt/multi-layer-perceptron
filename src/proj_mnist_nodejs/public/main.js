function trim(c) {
    var ctx = c.getContext('2d'),
      copy = document.createElement('canvas').getContext('2d'),
      pixels = ctx.getImageData(0, 0, c.width, c.height),
      l = pixels.data.length,
      i,
      bound = {
        top: null,
        left: null,
        right: null,
        bottom: null
      },
      x, y;
  
    for (i = 0; i < l; i += 4) {
      if (pixels.data[i+3] !== 0) {
        x = (i / 4) % c.width;
        y = ~~((i / 4) / c.width);
    
        if (bound.top === null) {
          bound.top = y;
        }
        
        if (bound.left === null) {
          bound.left = x; 
        } else if (x < bound.left) {
          bound.left = x;
        }
        
        if (bound.right === null) {
          bound.right = x; 
        } else if (bound.right < x) {
          bound.right = x;
        }
        
        if (bound.bottom === null) {
          bound.bottom = y;
        } else if (bound.bottom < y) {
          bound.bottom = y;
        }
      }
    }
      
    var trimHeight = bound.bottom - bound.top,
        trimWidth = bound.right - bound.left,
        trimmed = ctx.getImageData(bound.left, bound.top, trimWidth, trimHeight);
    copy.canvas.width = trimWidth;
    copy.canvas.height = trimHeight;
    copy.putImageData(trimmed, 0, 0);
    // open new window with trimmed image:
    return copy.canvas;
  }

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

    function showChart ( data ) {
        Chart.defaults.global.tooltips.enabled = false;

        var $graphicCanvas = document.getElementById("Graphic"),
            ctx = $graphicCanvas.getContext('2d'),
            myChart = new Chart(ctx, {
                type: 'bar',
                data: {
                    labels: ["0", "1", "2", "3", "4", "5", '6', '7','8', '9'],
                    datasets: [{
                        label: '',
                        data: data,
                        borderWidth: 1
                    }]
                },
                options: {
                    scales: {
                        yAxes: [{
                            ticks: {
                                beginAtZero:true
                            }
                        }]
                    }
                }
            });
    }


    window.recognize.addEventListener('click', function () {
        let img = new Image(28, 28),
            $trimCanvas = trim($canvas),
            trimContext = $trimCanvas.getContext("2d");
        
        img.onload = function () {            
            trimContext.clearRect(0, 0, 250, 250);
            trimContext.drawImage(img, 0, 0, 250, 250, 0, 0, 48, 48);
            
            let imgData = trimContext.getImageData(0, 0, 28, 28),
                pixels = imgData.data,
                xhr = new XMLHttpRequest(),
                newPixels = [];
            
            for ( let index = 0; index < pixels.length; index += 4 ) {
                let brightness = (pixels[index] +  pixels[index + 1] + pixels[index + 2]) / 3;
                
                newPixels.push(brightness / 255);

                pixels[index] = brightness;
                pixels[index + 1] = brightness;
                pixels[index + 2] = brightness;
            }

            context.putImageData(imgData, 0, 0);
            
            xhr.open('POST', 'http://localhost:8000', true);

            xhr.onreadystatechange = function () {
                if ( this.readyState !== 4 ) return;

                let result = JSON.parse(this.responseText),
                    item = Math.max.apply(null, result),
                    index = result.indexOf(item);
                
                window.res.value = index === -1 ? "Not recognized" : index;
                
                showChart(result);
            };

            xhr.send(newPixels);
        }

        img.src = $trimCanvas.toDataURL();
        clickX = [];
        clickY = [];
        clickDrag = [];
    });
})();