var app = angular.module('statsVis', []);

app.directive("statsVis", function($parse, $window, HUBServiceSet, $q, $http) {
  return {
    restrict: "EA",
    scope: {
        statsData: '=',
    },
    link: function(scope, elem, attrs)
    {
        var fileData = [];          // an array of all loaded data files
        var fileDataCounter = 0;    // the number of dat files to be loaded
        var contentCanBeCreated = false; // flag to identify if the content has b

        var d3 = $window.d3;
        var svg = d3.select(".stats-graphics").append("svg");
        var loading = svg.append("g");
        var content = svg.append("g");
        var summary = content.append("g");
        var info = content.append("g");
        var infoBar = content.append("g");
        var color = d3.scale.category20();
        var zoom = d3.behavior.zoom()
                     .scaleExtent([1, Number.POSITIVE_INFINITY])
                     .on("zoom", handleZoomPan);

        svg.call(zoom);

        var tip = ($window.d3.tip)()
            .attr('class', 'd3-tip')
            .html(function(d)
            {
                if (typeof d.name != "undefined")
                {
                    var tipContent = "";

                    var timeMs = Math.round(d.timeMs * 100) / 100;

                    tipContent += "<strong>Name:</strong> " + d.name + "<BR>\n";
                    tipContent += "<strong>Time:</strong> " + timeMs + " ms";

                    return tipContent;
                }
            });

        svg.call(tip);

        function createCoordinateSystem()
        {
            var width = getWidth();
            var height = getHeight();

            var x = d3.scale.linear()
                .range([30, width - 40]);

            var y = d3.scale.linear()
                .range([10, height]);

            return {x: x, y: y};
        }

        function handleZoomPan()
        {
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
                .range([30, width - 40]);

            zoom.translate(t);

            infoBar.attr("transform", "translate(" + t[0] + ", 0)");

            d3.selectAll("rect")
                .style("stroke-width", 1)
                .attr("x", function(d) { return isNaN(d.offset) ? 0: x(d.offset * s); })
                .attr("width", function(d) { return isNaN(d.ds) ? 0 : x(d.ds * s); });
        }

        function getHeight() {
            return angular.element(document.getElementsByClassName('stats-graphics')[0]['clientHeight'])[0];
        }

        function getWidth() {
            return angular.element(document.getElementsByClassName('stats-graphics')[0]['clientWidth'])[0];
        }

        var updateSize = function()
        {
            var width = getWidth();
            var height = getHeight();

            // set svg size to fill all space
            svg.attr("width", width)
               .attr("height", height);
        }

        var getClearCanvas = function()
        {
            summary.selectAll("*").remove();
            info.selectAll("*").remove();
            infoBar.selectAll("*").remove();

            // get x and y axis
            var cs = createCoordinateSystem();
            var x = cs.x;
            var y = cs.y;

            // reset zoom variables
            zoom.scale(1);
            zoom.translate([0, 0]);
            zoom.x(x).y(y);
        }

        var getTimeFromStats = function(stat)
        {
            var res = [];

            if( stat.children == undefined )
            {
                if(stat.name != undefined && stat.values.timeMs != undefined)
                    res.push({name: stat.name, time: stat.values.timeMs});

                return res;
            }
            else
            {
                var num = stat.children.length;
                for(var i=0;i<num;++i)
                    res = res.concat(getTimeFromStats(stat.children[i]));

                return res;
            }
        }

        var showInfo = function(timeCategoryName)
        {
            // clear info content
            info.selectAll("*").remove();
            infoBar.selectAll("*").remove();

            // collect time data of given category
            var timeData = [];
            var totalTime = 0;

            var num = fileData.length;
            for(var i=0;i<num;++i)
            {
                var times = fileData[i].times;
                var numTimes = times.length;
                for(var j=0;j<numTimes;++j)
                {
                    var time = times[j];
                    if(time.name == timeCategoryName)
                    {
                        timeData.push({uri: fileData[i].uri, timeMs: time.time});
                        totalTime += time.time;
                    }
                }
            }

            // create plotting data
            var plotData = [];
            var num = timeData.length;
            var offset = 0;
            for(var i=0;i<num;++i)
            {
                var timeMs = timeData[i].timeMs;
                var ds = timeMs / totalTime;
                var uri = timeData[i].uri;

                plotData.push({offset: offset, ds: ds, name: uri, timeMs: timeMs, colorIdx: i%4});
                offset += ds;
            }

            // create coordinate system
            var width = getWidth();
            var height = getHeight();
            var x = d3.scale.linear().range([30, width - 40]);

            // create rectangles
            var rectangle = infoBar.selectAll("rect").data(plotData);
            rectangle.enter().append("rect")
                             .on("mouseover", tip.show)
                             .on("mouseout", tip.hide);
            rectangle.exit().remove();
            infoBar.selectAll("rect").style("stroke-width", 1)
                                  .attr("x", function(d) { return x(d.offset); })
                                  .attr("y", function(d) { return 440; })
                                  .attr("width", function(d) { return x(d.ds); })
                                  .attr("height", function(d) { return 40; })
                                  .attr("fill", function(d) { return color(d.colorIdx); });

            // set info text
            info.append("text").attr("x", 10)
                               .attr("y", 420)
                               .style("font-family", "Helvetica")
                               .style("font-size", "20px")
                               .style("font-weight", "bold")
                               .text("Time Distribution: " + timeCategoryName);
        }

        var totalCPUTimeBuckets = function()
        {
            var resTimes = [];
            var totalTime = 0;

            var num = fileData.length;
            for(var i=0; i<num; ++i)
            {
                var times = fileData[i].times;
                var numTimes= times.length;

                for(var j=0; j<numTimes; ++j)
                {
                    var name = times[j].name;
                    var time = times[j].time;

                    if(resTimes[name] == undefined)
                        resTimes[name] = 0;

                    resTimes[name] += time;
                    totalTime += time;
                }
            }

            return {times: resTimes, total: totalTime};
        }

        var totalCPUTime = function()
        {
            var num = fileData.length;

            var totalTime = 0;
            for(var i=0; i<num; ++i)
                totalTime += fileData[i].totalTime;

            return totalTime;
        }

        var onTimePieClick = function(d)
        {
            showInfo(d.name)
        }

        var createLoadingScreen = function()
        {
            loading.selectAll("*").remove();

            var width = getWidth();
            var height = getHeight();

            loading.append("text").attr("x", 0)
                            .attr("y", 0)
                            .attr("transform", "translate(" + (width/2) + ", " + (height/2) + ")")
                            .attr("text-anchor", "middle")
                            .style("font-family", "Helvetica")
                            .style("font-size", "30px")
                            .style("font-weight", "bold")
                            .text("Loading...");
        }

        var createSummary = function(offset)
        {
            summary.append("text").attr("x", offset.x)
                            .attr("y", offset.y + 20)
                            .style("font-family", "Helvetica")
                            .style("font-size", "20px")
                            .style("font-weight", "bold")
                            .text("Summary");
            offset.y += 50;
            summary.append("text").attr("x", offset.x + 20)
                            .attr("y", offset.y)
                            .style("font-family", "Helvetica")
                            .style("font-size", "16px")
                            .text("Total number of jobs: " + fileData.length);
            offset.y += 22;
            summary.append("text").attr("x", offset.x + 20)
                            .attr("y", offset.y)
                            .style("font-family", "Helvetica")
                            .style("font-size", "16px")
                            .text("Total CPU time: " + Math.round(totalCPUTime()) + " ms");

            // create chart of all time data
            var timeData = totalCPUTimeBuckets();
            var total = timeData.total;
            var times = timeData.times;
            var counter = 0;

            plotData = [];
            var dsOffset = 0;
            for(time in times)
            {
                var timeMs = times[time];
                var ds = timeMs / total * 2 * Math.PI;
                var colorIdx = counter % 4;
                counter ++;

                plotData.push({name: time, ds: ds, timeMs: timeMs, offset: dsOffset, colorIdx: colorIdx});

                dsOffset += ds;
            }

            // create visualization
            var data = plotData;
            //var color = d3.scale.ordinal().range(["red", "blue", "orange"]);
            var arc = d3.svg.arc().innerRadius(100).outerRadius(180)
                    .startAngle(function(d) {return d.offset;})
                    .endAngle(function(d) {return d.offset + d.ds;} );

            summary.selectAll("path").data(plotData).enter().append("path")
                   .on("mouseover", tip.show)
                   .on("mouseout", tip.hide)
                   .on('click', onTimePieClick)
                   .attr("d", arc)
                   .style("fill", function(d){return color(d.colorIdx);})
                   .attr("transform", "translate(" + (500) + ", 200)");
        }

        var createContent = function()
        {
            // clear fresh canvas
            getClearCanvas();

            // create summary paragraph
            createSummary({x: 10, y: 10});

            // show content and make loading invisible
            loading.transition().style("opacity",0).duration(400);
            content.transition().style("opacity",1).duration(400);
        }

        var checkIfContentCanBeGenerated = function()
        {
            if(contentCanBeCreated == false && fileDataCounter == fileData.length)
            {
                contentCanBeCreated = true;
                createContent();
            }
        }

        var addStats = function(stats)
        {
            var timeMs = stats.values.timeMs;
            var uri = stats.children[0].texts.uri;
            var times = getTimeFromStats(stats);

            fileData.push({totalTime: timeMs, uri: uri, times: times});
        }

        var loadStatsData = function(uri)
        {
            var acceptHeader = 'application/json';
            var defer = $q.defer();

            $http.get(uri + '/stats.json', {
                      headers: {'Accept': acceptHeader} })
                .success(function(value, httpStatus, headers, config)
                {
                    jsonData = value;

                    addStats(jsonData);

                    loadChildren(uri);

                    defer.resolve(value);
                })
                .error(function(value, httpStatus, headers, config) {
                    defer.reject(httpStatus);
                });

            return defer.promise;
        }

        var loadChildren = function(uri)
        {
            var acceptHeader = 'application/json';
            var defer = $q.defer();

            $http.get(uri + '/children.json', {
                      headers: {'Accept': acceptHeader} })
                .success(function(value, httpStatus, headers, config)
                {
                    children = value.children;
                    var numChildren = children.length;

                    fileDataCounter = fileDataCounter + numChildren;

                    for (var i = 0; i < numChildren; i++)
                        loadStatsData(children[i].url);

                    checkIfContentCanBeGenerated();

                    defer.resolve(value);
                })
                .error(function(value, httpStatus, headers, config)
                {
                    checkIfContentCanBeGenerated();
                    defer.reject(httpStatus);
                });

            return defer.promise;
        }

        var dataChanged = function(newValue, oldValue)
        {
            // no data loaded, or data doesn't exist, do nothing
            if (typeof newValue == "undefined" || newValue.status != 200) {
                return;
            }

            updateSize();
            createLoadingScreen();

            // make content invisible and show loading screen
            content.style("opacity", 0);
            loading.style("opacity", 1);

            // clear stats array
            fileData = [];
            contentCanBeCreated = false;

            // add json data
            fileDataCounter = 1;
            addStats(newValue.data);

            // load all children data
            loadChildren(HUBServiceSet.currService.jobSet.currentJob.uri);

        }

        scope.$watch('statsData', dataChanged);
    }
  };
});