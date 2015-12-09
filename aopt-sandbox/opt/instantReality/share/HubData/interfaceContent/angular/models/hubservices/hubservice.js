angular.module("myApp").factory("HUBService", ["$http", "$q", "JobSet", function($http, $q, JobSet){
    var HUBService = function(){

    };

    HUBService.prototype = {
        setData: function(key, json){
//            console.log(key);
//            console.log(json);
            
            this.host        = json.Host;
            this.serviceName = key;
            this.machine     = json.MachineType;
            this.cpu_count   = json.CPUs;
            this.os          = json.OS;
            this.delivery    = json.Delivery;
            this.version     = json.Version;
            this.license     = json.Licensed;
            this.threads     = json.NumThreads;

            var serviceProvider = json;
//            console.log(serviceProvider);
            this.id = serviceProvider.id;

            if(window.location.protocol == 'https:')
            {
                this.resource = 'https://'+serviceProvider.LinkSSL+serviceProvider.JobUri;
            }
            else
            {
                this.resource = 'http://'+serviceProvider.Link+serviceProvider.JobUri;
            }

            this.status = serviceProvider.status;
            this.options = serviceProvider.options;
            this.system = serviceProvider.system;
            this.jobUri = serviceProvider.JobUri;
            this.jobSet = new JobSet();
            this.link = serviceProvider.Link;
            this.linkSSL = serviceProvider.LinkSSL;
            this.port = serviceProvider.Port;
            this.portssl = serviceProvider.PortSSL;
        },
        updateData: function(json){
//            console.log(json);
            
            this.host        = json.Host;
//            this.serviceName = key;
            this.machine     = json.MachineType;
            this.cpu_count   = json.CPUs;
            this.os          = json.OS;
            this.delivery    = json.Delivery;
            this.version     = json.Version;
            this.license     = json.Licensed;
            this.threads     = json.NumThreads;
            
//            var serviceProvider = json;
//            console.log(serviceProvider);
//            this.id = serviceProvider.id;
//            this.resource = 'http://'+serviceProvider.Link+serviceProvider.JobUri;
//            this.status = serviceProvider.status;
//            this.options = serviceProvider.options;
//            this.system = serviceProvider.system;
//            this.jobUri = serviceProvider.JobUri;
//            this.link = serviceProvider.Link;
//            this.port = serviceProvider.Port;
        },
        toJSON: function(){
            return({
                host: this.host,
                servicename: this.name,

                id: this.id,
                resource: this.resource,
                status: this.status,
                options: this.options,
                system: this.system,
                joburi: this.jobUri,
                link: this.link,
                port: this.port
            });
        },
        updateInfo: function($http) {
//            console.log("update s info");
//            console.log(this.link)

            var that = this;

            var uri = 'http://' + this.link;

            if(window.location.protocol == 'https:')
            {
                uri = 'https://' + this.linkSSL;
            }

            $http.get(uri,
                      { headers:  {
                          'Accept': 'application/json',
                      } }).success(function(data, status, headers, config) {
                      }).then(function(res){
//                          console.log(res);
//                          console.log(that);
                          that.updateData(res.data);
                      });
        },
        getJobSetPromise: function() {         
            var that = this;
            var defer = $q.defer();

            $http.get(that.resource,
                { headers: { 'Accept': 'application/json' } 
            }).success(function(data) {
                if (that.jobSet.elements.length > 0) {
                    that.jobSet.updateJobs(data);
                } else {
                    that.jobSet.addAllJobs(data);                     
                }
                defer.resolve(that.jobSet);
            }).error(function(data, status, headers, config) {
                defer.reject(status);
            }).then(function() {
                return that.jobSet;
            });
            
            return defer.promise;
            
        }                   
    }
    return HUBService;
}]).service("HUBServiceSet", function(HUBService){
    this.elements = [];
    this.serviceChangedListeners = [];
    this.currService = null;
    
    this.create = function(key, providers){
//        console.log("foob");
//        console.log(providers);
        for(var p in providers) {
//            console.log(providers[p]);
            var newS = new HUBService();
            newS.setData(key, providers[p]);
            this.elements.push(newS);
//            console.log(this.elements);
        }
    }

    this.createAll = function(list){
        for(var l in list) {
            this.create(l, list[l]['ServiceProvider']);
        }
    }

    this.updateServicesInfo = function($http) {
//        console.log("update services infos");

        for(var l in this.elements) {
//            console.log(this.elements[l]);
            this.elements[l].updateInfo($http);
        }
    }
    
    this.addServiceChangedListener = function(listener) {
        var contains = this.serviceChangedListeners.indexOf(listener) >= 0;
        if (!contains) {
            this.serviceChangedListeners.push(listener);
        }
    }
    
    this.removeServiceChangedListener = function(listener) {
        var containsIndex = this.serviceChangedListeners.indexOf(listener);
        if (containsIndex >= 0) {
            this.serviceChangedListeners.splice(containsIndex, 1);
        }        
    }
    
    this.setCurrentService = function(newService) {
        var oldService = this.currService;
        this.currService = newService;
        
        for (listener in this.serviceChangedListeners) {
            this.serviceChangedListeners[listener](oldService, newService);
        }
    }


});
