
function resolveRelative(path, base) {
    // Upper directory
    if (path.indexOf("../") == 0) {
        return resolveRelative(path.slice(3), base.replace(/\/[^\/]*$/, ''));
    }
    // Relative to the root
    if (path.indexOf('/') == 0) {
        var match = base.match(/(\w*:\/\/)?[^\/]*\//) || [base];
        return match[0] + path.slice(1);
    }
    //relative to the current directory
    return base.replace(/\/[^\/]*$/, "") + '/' + path;
}

angular.module('myApp').controller('JobStatisticController', function($scope, $window, $http)
{
    // job statistic update interval in milliseconds
    var JOB_STATISTIC_UPDATE_INTERVAL = 10000;

    // get the div environment containing the graph element
    var graph = d3.select(".jobStatisticDataDiv");

    // create svg object which contains the job statistic graph
    var svg = graph.append("svg").attr("width", 550).attr("height", 250);

    // references for the chart and data object instances
    var chart;
    var chartData;

//    console.log(window.location.href);

    var statPath = resolveRelative("job/statistic", $window.location.href); 

//    console.log(statPath);

    /**
     * Sends a http request to the instant hub in order to get the
     * workload over time.
     */
    var requestNewJobStats = function()
    {
        // send request to hub for statistical data about the jobs
/*
        $http.get($window.location.protocol + '//' + $window.location.hostname + ($window.location.port ? ':' +
                                                                            $window.location.port:
                                                                           '')  + '/job/statistic',
              { headers:  {
                  'Accept': 'application/json',
              } }).success(function(data, status, headers, config) {
    }).then(function(res)
*/
        $http.get(statPath,
              { headers:  {
                  'Accept': 'application/json',
              } }).success(function(data, status, headers, config) {
    }).then(function(res)
        {
            var workload = res.data.workload;
            var numEntries = workload.length;

            var openJobs = [];

            for(var i=0;i<numEntries;++i)
            {
                var w = workload[i];
                var numOpenJobs = w.jobStats.numOpenJobs;

                openJobs.push({x: Date.parse(w.timestamp), y: numOpenJobs});
            }

            var data =
            [
                {
                  values : openJobs,
                  key : "Open Jobs"
                }
            ];

            chartData.datum(data).transition().duration(350).call(chart);
            nv.utils.windowResize(chart.update);

        });
    }

    // create chart object and configure it
    nv.addGraph(function()
    {
        // create empty chart object without data
        chart = nv.models.stackedAreaChart()
                      .margin({right: 10, bottom: 90})
                      //.transitionDuration(500)
                      .width(550)
                      .height(250)
                      .showControls(false)       //Allow user to choose 'Stacked', 'Stream', 'Expanded' mode.
                      .clipEdge(false);

        // define formating for the x and y axis
        chart.xAxis.tickFormat
        (
            function(d)
            {
                return d3.time.format('%X')(new Date(d));
            }
        );

        chart.yAxis.tickFormat(d3.format('d'));

        // set empty dummy data
        var dummyData =
        [
            {
                values : [ ],
                key : "Open Jobs"
            }
        ];

        chartData = svg.datum(dummyData);
        chartData.call(chart);

        nv.utils.windowResize(chart.update);

        requestNewJobStats();
    });

    window.setInterval(requestNewJobStats, JOB_STATISTIC_UPDATE_INTERVAL);
})
