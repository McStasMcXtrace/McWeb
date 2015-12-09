angular.module('myApp').controller('hubController', function($scope, $http, Hub, HubSet, $window){

    this.currHub = '';
    this.showDetails = false;

    $scope.resource = '';

    var that = this;
    this.hub = HubSet;

//    console.log(this.hub);

    this.setHub = function(hub){
        this.hub.currHub = hub;
    }

    this.postClearCache = function(){
        console.log("postClearCache");

        $http.get(window.location.protocol + '//' + window.location.hostname + (window.location.port ? ':' +
                                                                                window.location.port : '') + 
                  '/clearCache');
    }

    this.postDeliveryInternal = function() {
        console.log("postDeliveryInternal");

        $http.get(window.location.protocol + '//' + window.location.hostname + (window.location.port ? ':' +
                                                                                window.location.port : '') + 
                  '/setDelivery?Hermes=1');
    }

    this.postDeliveryNginxHttp = function() {
        console.log("postDeliveryNginxHttp");

        $http.get(window.location.protocol + '//' + window.location.hostname + (window.location.port ? ':' +
                                                                                window.location.port : '') + 
                  '/setDelivery?nginxhttp=1');
    }

    this.postDeliveryNginxHttps = function() {
        console.log("postDeliveryNginxHttps");

        $http.get(window.location.protocol + '//' + window.location.hostname + (window.location.port ? ':' +
                                                                                window.location.port : '') + 
                  '/setDelivery?nginxhttps=1');
    }

    this.postDeliveryInternalByIp = function() {
        console.log("postDeliveryInternalByIp");

        $http.get(window.location.protocol + '//' + window.location.hostname + (window.location.port ? ':' +
                                                                                window.location.port : '') + 
                  '/setDelivery?Hermes=1&localip=1');
    }

    this.postDeliveryNginxHttpByIp = function() {
        console.log("postDeliveryNginxHttpByIp");

        $http.get(window.location.protocol + '//' + window.location.hostname + (window.location.port ? ':' +
                                                                                window.location.port : '') + 
                  '/setDelivery?nginxhttp=1&localip=1');
    }

    this.postDeliveryNginxHttpsByIp = function() {
        console.log("postDeliveryNginxHttpsByIp");

        $http.get(window.location.protocol + '//' + window.location.hostname + (window.location.port ? ':' +
                                                                                window.location.port : '') + 
                  '/setDelivery?nginxhttps=1&localip=1');
    }

    var hubPath = resolveRelative("hub", $window.location.href); 

//    console.log(hub);

/*
    $http.get(window.location.protocol + '//' + window.location.hostname + (window.location.port ? ':' +
                                                                            window.location.port:
                                                                           '')  + '/hub',
              { headers:  {
                  'Accept': 'application/json',
              } }).success(function(data, status, headers, config) {
    }).then(function(res){
*/
    $http.get(hubPath,
              { headers:  {
                  'Accept': 'application/json',
              } }).success(function(data, status, headers, config) {
    }).then(function(res){
        var received_hub = res.data;
//        console.log("bar");
//        console.log(received_hub);

        
        that.hub.create(received_hub);
        that.hub.currHub = that.hub.elements[0];
        });
})
