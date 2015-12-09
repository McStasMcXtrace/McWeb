angular.module('myApp').controller('hubServicesController', function($scope, $http, HUBService, HUBServiceSet, $window){
    var that = this;
    
    this.viewScopes = {
        SERVICE_LIST: 'service_overview',
        JOB_LIST: 'job_list',
        JOB_DETAILS: 'job_details'
    };

    $scope.currentViewScope = this.viewScopes.SERVICE_LIST;
    
    this.setViewScope = function(viewScope) {
        $scope.currentViewScope = viewScope;
    };
    
    this.getViewScope = function() {
        return $scope.currentViewScope;
    };
    
    this.toggleJobList = function() {
        var currentScope = this.getViewScope();
        
        switch(currentScope) {
            case this.viewScopes.SERVICE_LIST:
                this.setViewScope(this.viewScopes.JOB_LIST);
                break;
            case this.viewScopes.JOB_LIST:
                this.setViewScope(this.viewScopes.SERVICE_LIST);
                break;
            default:
                break;
        }
    };
    

    this.services = HUBServiceSet;
    
    this.jobChangedListener = function(oldJob, newJob) {      
        if (newJob == null) {
            that.setViewScope(that.viewScopes.SERVICE_LIST);
        } else {
            that.setViewScope(that.viewScopes.JOB_DETAILS);
        }
    };
    
    this.services.addServiceChangedListener(function(oldService, newService) {
        if (oldService != null) {
            oldService.jobSet.removeJobChangedListener(that.jobChangedListener);
        }
        if (newService != null) {
            newService.jobSet.addJobChangedListener(that.jobChangedListener);
        }
    });

    var servicePath = resolveRelative("service", $window.location.href); 

/*
    $http.get(window.location.protocol + '//' + window.location.hostname + (window.location.port ? ':' +
                                                                            window.location.port:
                                                                            '')  + '/service',
              { headers:  {
                  'Accept': 'application/json',
              } }).success(function(data, status, headers, config) {
*/
    $http.get(servicePath,
              { headers:  {
                  'Accept': 'application/json',
              } }).success(function(data, status, headers, config) {
    }).then(function(res){
            var received_services = res.data;
//            console.log("foo");
//            console.log(received_services);
            that.services.createAll(received_services);
            that.services.setCurrentService(that.services.elements[0]);
        });
})
