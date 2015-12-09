var app = angular.module('structTreeVis', []);

app.directive("structTreeVis", function($parse, $window) {
  return {
    restrict: "EA", 
    scope: {
        graphData: '=',
    },
    link: function(scope, elem, attrs) {
        var useClickInteraction = false;
        
        var d3 = $window.d3;
        var color = d3.scale.category10();

        var partition = d3.layout.partition()
            .children(function(d) { return d.children; })
            .value(function(d) {
                if (typeof d.volume == "undefined")
                    return 0;

                return getVolume(d.volume); 
            });
        
        var getVolume = function(bb) {
            var x1 = bb[0]; var x2 = bb[3];
            var y1 = bb[1]; var y2 = bb[4];
            var z1 = bb[2]; var z2 = bb[5];

            return (x2 - x1) * (y2 - y1) * (z2 - z1);
        };
        
            
        var svg = d3.select(".struct-graphics").append("svg");       
        var g = svg.append("g");
        var legend = svg.append("g");
                
        var tip = ($window.d3.tip)()
            .attr('class', 'd3-tip')
            .html(function(d) {
                if (typeof d.dataRef != "undefined")
                    return "<strong>Data Ref:</strong> " + decodeURIComponent(d.dataRef[0]);
                if (typeof d.label != "undefined")
                    return "<strong>Name:</strong> " + decodeURIComponent(d.label);
                return "<strong>ID:</strong> " + d.id;
            });
            
        svg.call(tip);
        
        if (!useClickInteraction) {
            var zoom = d3.behavior.zoom()            
                .scaleExtent([1, Number.POSITIVE_INFINITY])
                .on("zoom", handleZoomPan);
            
            svg.call(zoom);
        }
        
        function handleZoomPan() {
            var t = d3.event.translate;
            var s = d3.event.scale; 

            var width = getWidth();
            var height = getHeight();
            
            var h = height/4;

            t[0] = Math.min(
                (width/height) * (s - 1), 
                Math.max(width * (1 - s), t[0] )
            );

            t[1] = Math.min(
                h * (s - 1) + h * s, 
                Math.max(height * (1 - s) - h * s, t[1])
            );
            
            var x = d3.scale.linear()
                .range([0, width]);

            var y = d3.scale.linear()
                .range([0, height]);

            zoom.translate(t);
            
            g.attr("transform", "translate(" + t[0] + ", 0)");
            //g.attr("transform", "translate(" + t[0] + ", 0) scale(" + s + ", 1)");
            
            drawLegend(x, y, s);
            
            d3.selectAll("rect")
                .style("stroke-width", 1)     
                .attr("x", function(d) { return isNaN(d.x) ? 0: x(d.x * s); })
                .attr("y", function(d) { return isNaN(d.y) ? 0: y(d.y); })
                .attr("width", function(d) { return isNaN(d.dx) ? 0 : x(d.dx * s); })
                .attr("height", function(d) { return isNaN(d.dy) ? 0 : y(d.dy); });
        }
        
        function getHeight() {
            return angular.element(document.getElementsByClassName('struct-graphics')[0]['clientHeight'])[0];
        }
        
        function getWidth() {
            return angular.element(document.getElementsByClassName('struct-graphics')[0]['clientWidth'])[0];
        }
        
        function rebuildTree(graphData) {
            // clear old tree
            g.selectAll("*").remove();
            
            var width = getWidth();
            var height = getHeight();
            
            // set svg size to fill all space
            svg.attr("width", width)
               .attr("height", height);
               
            var x = d3.scale.linear()
                .range([0, width]);

            var y = d3.scale.linear()
                .range([0, height]);
                
            if (!useClickInteraction) {
                // reset zoom variables
                zoom.scale(1);
                zoom.translate([0, 0]);
                zoom.x(x).y(y);
                // remove applied transforms
                g.attr("transform", "");
            } 
            
            drawLegend(x, y, 1);
                
            // refill tree with data
            var rectangle = g.selectAll("rect")
                .data(partition(graphData.data.node.children[0]));
                
            rectangle.enter().append("rect")
                .on("click", useClickInteraction ? clicked : function(d) { return; })
                .on("mouseover", tip.show)
                .on("mouseout", tip.hide);
                
            rectangle.exit().remove();
            
            rectangle.attr("x", function(d) { return x(d.x); })
                .attr("y", function(d) { return y(d.y); })
                .attr("width", function(d) { return x(d.dx); })
                .attr("height", function(d) { return y(d.dy); })
                .attr("fill", function(d) { return color(typeof d.dataRef != "undefined"); });
 
            function clicked(d) {
                x.domain([Math.max(0, d.x - d.dx/2), Math.min(1, d.x + d.dx + d.dx/2)]);
                y.domain([d.y ? d.y - d.dy/2 : 0, 1]);
                
                drawLegend(x, y, 1 / (x.domain()[1] - x.domain()[0]));
                
                // center view around clicked node
                rectangle.transition()
                    .duration(200)
                    .attr("x", function(d) { return x(d.x); })
                    .attr("y", function(d) { return y(d.y); })
                    .attr("width", function(d) { return x(d.x + d.dx) - x(d.x); })
                    .attr("height", function(d) { return y(d.y + d.dy) - y(d.y); });
            } 
        }
        
        var drawLegend = function(x, y, scale) {
            var text = legend.selectAll("text")
                .data([scale]);
                
            text.enter().append("text");
            
            text.attr("x", x.range()[0] + 20)
                .attr("y", y.range()[1] - 20)
                .style("font-family", "Helvetica")
                .style("font-size", "16px")
                .text(function(d) { return "Zoom level: " + scale.toFixed(1) + 'X'; });

            //TODO: draw overview to show user current position
        }
        
        var currentData;
        var dataChanged = function(newValue, oldValue) {               
            // clean up graph, since data changed
            g.selectAll("*").remove();
            
            // no data loaded, or data doesn't exist, do nothing
            if (typeof newValue == "undefined" || newValue.status != 200) {
                return;
            } 
            
            currentData = newValue;
            rebuildTree(newValue);
        }

        scope.$watch('graphData', dataChanged);
        
        scope.$watch(function() {
            return $window.innerWidth;
        }), function(value) {
            console.log(value);
            dataChanged(currentData);
        }
    }
  };
});