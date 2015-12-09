angular.module('myApp').controller('jobController', ['$scope', '$http', 'Job', 'JobSet', 'HUBServiceSet', '$q', function($scope, $http, Job, JobSet, HUBServiceSet, $q){
    
    this.JobTabs = {
        LOG: 'log',
        ACC: 'acc',
        STRUCT: 'struct',
        GENERAL: 'general'
    };
    
    this.activeJobTab = this.JobTabs.LOG;
    var that = this;
    
    var tabLoadingFunctions = [];
    tabLoadingFunctions[this.JobTabs.LOG] = function() {
        loadFile(that.JobTabs.LOG, '/log.txt', 'text/plain')
            .then(function(data) {
                
            }, function(errorStatus) {
                if (errorStatus == 404) {
                    loadFile(that.JobTabs.LOG, '/console.txt', 'text/plain')
                }
            });
    };
    tabLoadingFunctions[this.JobTabs.ACC] = function() {
        loadFile(that.JobTabs.ACC, '/acc/tree.hsi', 'application/json')
            .then(function(data) {
                
            }, function(errorStatus) {
                
            });
    };
    tabLoadingFunctions[this.JobTabs.STRUCT] = function() {
        loadFile(that.JobTabs.STRUCT, '/struct/tree.hsi', 'application/json')
            .then(function(data) {
                
            }, function(errorStatus) {
                
            });
    };   
    tabLoadingFunctions[this.JobTabs.GENERAL] = function() {
        loadFile(that.JobTabs.GENERAL, '/stats.json', 'application/json')
            .then(function(data) {
                
            }, function(errorStatus) {
                
            });
    };

    var loadFile = function(type, statisticsSuffix, acceptHeader) {
        var currentJob = HUBServiceSet.currService.jobSet.currentJob;      
        var defer = $q.defer();
        
        $http.get(currentJob.uri + statisticsSuffix, {
                  headers: {'Accept': acceptHeader} })
            .success(function(value, httpStatus, headers, config) {
                currentJob.statistics[type] = {
                    status: httpStatus, 
                    data: value
                };
                defer.resolve(value);
            })
            .error(function(value, httpStatus, headers, config) {
                currentJob.statistics[type] = {
                    status: httpStatus, 
                    data: 'Error: ' + httpStatus + '. Not all jobs have the same information available.'
                };
                defer.reject(httpStatus);
            });
            
        return defer.promise;
    }

    this.setActiveJobTab = function(tab) {
        this.activeJobTab = tab;
        tabLoadingFunctions[tab]();
    };
    
    this.getActiveJobTab = function() {
        return this.activeJobTab;
    };
    
    var jobChangedListener = function(oldJob, newJob) {
        if (newJob != null) {
            tabLoadingFunctions[that.getActiveJobTab()]();           
        }
    };
    
    HUBServiceSet.addServiceChangedListener(function(oldService, newService) {
        if (oldService != null) {
            oldService.jobSet.removeJobChangedListener(jobChangedListener);            
        }
        if (newService != null) {
            newService.jobSet.addJobChangedListener(jobChangedListener);
        }
    });
}]);
