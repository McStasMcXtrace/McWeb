'use strict';


angular.module('app.controllers', []).controller('AnchorListCtrl', ['$scope', '$rootScope', function($scope, $rootScope) {
  $scope.data = [];
  for (var i=0; i<20; i++) {
    $scope.data[i] = i;
  }
}]);

angular.module('myApp.ScrollSpyDirectives', []).directive("scrollTo", ["$window", function($window){
  return {
    restrict : "AC",
    compile : function(){

      function scrollInto(elementId) {
        if(!elementId) $window.scrollTo(0, 0);
        //check if an element can be found with id attribute
        var el = document.getElementById(elementId);

        /*remove class, then add*/
        var list = document.getElementsByClassName('sidebar_li');
        for(var i =0; i<list.length;i++){
          list[i].classList.remove('active');
        }
        var parent = document.getElementById('li_'+elementId);
        parent.classList.add('active');

        if(el) el.scrollIntoView();
      }

      return function(scope, element, attr) {
        element.bind("click", function(event){
          scrollInto(attr.scrollTo);
        });
      };
    }
  };
}]).directive('scrollSpy', ['$window', '$timeout', '$rootScope', function($window, $timeout, $rootScope) {
  var targets, spies = [];

  var refresh = function(attrs) {
    var slice = Array.prototype.slice;

    $timeout(function() {
      targets = $(attrs.target).children();
      slice.call(targets).forEach(function(el) {
        var spy = $(el.querySelector('a:first-child').getAttribute('href'));
        if (spy.length > 0) {
          spies.push(spy);
        }
      });
    }, 1000);
  };

  var activate = function(scope, $element, attrs) {
    $(attrs.target + ' > .active').removeClass('active');
    $element.addClass('active');
  };

  var process = function(scope, element, attrs) {
    var windowHeight = $window.innerHeight,
        windowTop = $window.scrollY,
        $activeTarget;

    spies.map(function(item, index) {
      var pos = item.offset().top - windowTop;
      console.log(pos);
      if (pos < windowHeight) {
        $activeTarget = targets.eq(index);
      }
    });

    activate(scope, $activeTarget, attrs);
  };

  return {
    link: function(scope, element, attrs) {
      targets = [];
      spies = [];

      refresh(attrs);

      angular.element(element).bind("mousewheel", function() {  
        process(scope, element, attrs);
        scope.$apply();
      });

      // When DOM changes, refresh with a broadcast like this $rootScope.$broadcast('scrollspy.refresh');
      $rootScope.$on('scrollspy.refresh', function() {
        refresh(attrs);
      });

    }
  }
}]);
