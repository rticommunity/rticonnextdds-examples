/*
 * (c) 2016-2017 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

var rti = rti || {};

/**
 * @namespace rti.shapesdemo
 */
rti.shapesdemo = {
    /**
     * Sets up a new canvas in a <div id="shapesDemoCanvas">. This method
     * needs to be called before reading or drawing shapes.
     */
    setupScenario: function() {
        rti.shapesdemo.canvas = new fabric.Canvas(
            'shapesDemoCanvas',
            {
                hoverCursor: 'pointer',
                perPixelTargetFind: true,
                targetFindTolerance: 5,
                backgroundImage: '../../../../resources/img/rti_background.png'
            }
        );
    },

    /**
     * Sets up the enviroment for reading shapes. The method will call the
     * methods that draw shapes at 33 ms intervals.
     */
    readShapes: function() {
        var squareReaderUrl =
            "/dds/rest1/applications/ShapesDemoApp" +
            "/domain_participants/MyParticipant" +
            "/subscribers/MySubscriber" +
            "/data_readers/MySquareReader";
        var triangleReaderUrl =
            "/dds/rest1/applications/ShapesDemoApp" +
            "/domain_participants/MyParticipant" +
            "/subscribers/MySubscriber" +
            "/data_readers/MyTriangleReader";
        var circleReaderUrl =
            "/dds/rest1/applications/ShapesDemoApp" +
            "/domain_participants/MyParticipant" +
            "/subscribers/MySubscriber" +
            "/data_readers/MyCircleReader";

        var shapesDemoIntervalPeriod = 33; // in milliseconds

        // Call drawShape() for Squares, Circles, and Triangles every
        // shapesDemoIntervalPeriod, passing the resulting data 
        // for reading new samples of the appropriate topic in json format
        // without deleting the samples from the Reader's cache.
        setInterval(function(){
            // Read Squares
            $.getJSON(
                squareReaderUrl,
                {
                    sampleFormat: "json",
                    removeFromReaderCache: "false"
                },
                function(data) {
                    rti.shapesdemo.drawShape(data, "Square");
                }
            );

            // Read Triangles
            $.getJSON(
                triangleReaderUrl,
                {
                    sampleFormat: "json",
                    removeFromReaderCache: "false"
                },
                function(data) {
                    rti.shapesdemo.drawShape(data, "Triangle");
                }
            );

            // Read Circles
            $.getJSON(
                circleReaderUrl,
                {
                    sampleFormat: "json",
                    removeFromReaderCache: "false"
                },
                function(data) {
                    rti.shapesdemo.drawShape(data, "Circle");
                }
            );
        }, shapesDemoIntervalPeriod);
    },

    /**
     * Draws a sequence of shapes given their shape kind (i.e., whether it is
     * a Circle, Square, or a Triangle).
     * @param sampleSeq Sequence of samples to be drawn.
     * @param shapeKind kind of shape to draw. Valid values are 'Circle',
     * 'Square', or 'Triangle'.
     */
    drawShape: function(sampleSeq, shapeKind) {
        sampleSeq.forEach(function(sample, i, samples) {
            // Process metadata
            var validData = sample.read_sample_info.valid_data;
            var instanceHandle = sample.read_sample_info.instance_handle;
            var instanceState  = sample.read_sample_info.instance_state;

            // If we received an invalid data sample, and the instance state
            // is != ALIVE, then the instance has been either disposed or
            // unregistered and we remove the shape from the canvas.
            if (!validData) {
                if (instanceState != "ALIVE") {
                    rti.shapesdemo.canvas.getObjects().every(
                        function (element, j, array) {
                            if (element.uid.instanceHandle == instanceHandle
                                && element.uid.topic == shapeKind) {
                                element.remove();
                                rti.shapesdemo.canvas.renderAll();
                                return false;
                            }
                            return true;
                        }
                    );
                    return true;
                }
                return true;
            }

            // Process sample data (i.e., color, shapesSize, and position
            // of the received shape). If the color is not in the list of
            // supported colors, we assign it blue.
            var color;
            if (rti.shapesdemo.shapeColors.hasOwnProperty(sample.data.color)) {
                color = rti.shapesdemo.shapeColors[sample.data.color];
            } else {
                color = rti.shapesdemo.shapeColors.BLUE;
            }

            var shapeSize = sample.data.shapesize;
            var x = sample.data.x - shapeSize/2;
            var y = sample.data.y - shapeSize/2;

            // Look for the shape received on the canvas
            var shape = undefined;
            for (var i = 0; i < rti.shapesdemo.canvas.getObjects().length; i++){
                var currentElement = rti.shapesdemo.canvas.getObjects()[i];
                if (currentElement.uid.instanceHandle == instanceHandle
                        && currentElement.uid.topic == shapeKind) {
                    shape = currentElement;
                    break;
                }
            }

            // If it is not there, we need to create a new shape in the
            // received position. Otherwise, update the existing shape's
            // position.
            if (shape == undefined) { // Add new shape in the position
                var newShape;
                if (shapeKind == "Square") {
                	newShape = new fabric.Rect({
                	    left: x,
                	    top: y,
                	    originX: 'left',
                	    originY: 'top',
                	    width: shapeSize,
                	    height: shapeSize,
                	    angle: 0,
                            fill: color,
                            transparentCorners: false,
                            selectable: false
                        });
                } else if (shapeKind == "Triangle") {
                    newShape = new fabric.Triangle({
                	    left: x,
                	    top: y,
                	    originX: 'left',
                	    originY: 'top',
                	    width: shapeSize,
                	    height: shapeSize,
                	    angle: 0,
                	    fill: color,
                            transparentCorners: false,
                            selectable: false
                	});
                } else if (shapeKind == "Circle"){
                    newShape = new fabric.Circle({
                	    left: x,
                	    top: y,
                	    originX: 'left',
                	    originY: 'top',
                	    radius: shapeSize/2,
                	    angle: 0,
                	    fill: color,
                            transparentCorners: false,
                            selectable: false
                	});
                }
                newShape.uid = {
                    "instanceHandle": instanceHandle,
                    "topic": shapeKind
                }
                rti.shapesdemo.canvas.add(newShape);
            } else { // Update position of existing shape
                shape.setLeft(x).setCoords();
                shape.setTop(y).setCoords();
                if (shapeKind == "Circle"){
                    shape.setRadius(shapeSize/2);
                } else {
                    shape.setWidth(shapeSize);
                    shape.setHeight(shapeSize);
                }
                rti.shapesdemo.canvas.renderAll();
            }
            return true;
        });
    }
}

/**
 * Canvas in which we will be drawing all the shapes
 */
rti.shapesdemo.canvas = {};

/**
 * Object with the list of supported colors along with their corresponding
 * color code.
 */
rti.shapesdemo.shapeColors = {
    PURPLE : "#b000ff",
    BLUE: "#0000ff",
    RED: "#ff0000",
    GREEN: "#00ff00",
    YELLOW: "#ffff00",
    CYAN: "#00ffff",
    MAGENTA: "#ff00ff",
    ORANGE: "#ff8200"
};
