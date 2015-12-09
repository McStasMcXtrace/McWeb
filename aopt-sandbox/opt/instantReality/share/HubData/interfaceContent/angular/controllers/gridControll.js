angular.module('myApp').controller('gridCtrl', ['$scope', '$http', 'uiGridConstants', 'HUBServiceSet', '$interval', function ($scope, $http, uiGridConstants, HUBServiceSet, $interval) {

    var that = this;
    var failedJobs = 0;
    this.loaded = false;
    this.loading = false;
    this.autoRefresh = false;
    
    var gridTextFilterFunc = function(searchTerm, cellValue) {
        if (searchTerm != '') {
            var normalizedCellValue = cellValue.toLowerCase();
            // characters like '.' and '-' are being escaped... remove it
            var normalizedSearchTerm = searchTerm.toLowerCase().replace(/\\/g, '');

            return normalizedCellValue.match(normalizedSearchTerm);
        }
        return true;
    };

  //GRID OPTIONS
    $scope.gridOptions = {
      enableRowSelection: true,
      enableRowHeaderSelection: false,
	    enableFiltering: true,
      enablePaginationControls: false,
      multiSelect: false,
      flatEntityAccess: true,
      paginationPageSize: 1e12,
      hideGrid: false,
      columnDefs: [
        { 
          name: 'ID', 
          field: 'id',  
          filter: { condition: gridTextFilterFunc },
          sortingAlgorithm: function(a, b) {
              var nulls = $scope.gridApi.core.sortHandleNulls(a, b);
              if (nulls !== null) {
                  return nulls;
              } else {
                  var numA = parseInt(a.replace(/job/, ''));
                  var numB = parseInt(b.replace(/job/, ''));
                  if (numA > numB) return 1;
                  if (numA < numB) return -1;
                  return 0;
              }
          }
        },
        { 
          name: 'resource', 
          field: 'resource', 
          filter: { condition: gridTextFilterFunc } 
        }, 
        { 
          name: 'status', 
          field: 'status', 
          filter: {
            type: uiGridConstants.filter.SELECT,
            selectOptions: [ 
              { value: 'OPEN',        label: 'Open'        }, 
              { value: 'INPROGESS',   label: 'In Progress' }, 
              { value: 'CLOSED',      label: 'Closed'      },
              { value: 'TERMINATED',  label: 'Terminated'  },
              { value: 'ERROR',       label: 'Error'       } ]
		      } 
        }
      ]
    };

    $scope.gridOptions.onRegisterApi = function (gridApi) {
      $scope.gridApi = gridApi;
      that.openJobDetail(gridApi);
    };

    this.openJobDetail = function(gridApi){
      gridApi.selection.on.rowSelectionChanged($scope,function(row){
          HUBServiceSet.currService.jobSet.setCurrentJob(row.entity);
      });
    }

    this.loadJobData = function() {
        // Don't try to load if we are still loading...
        if (that.loading == true)
            return;
        
        if (HUBServiceSet.currService != null) {
            that.loading = true;
            HUBServiceSet.currService.getJobSetPromise().then(function(jobSet) {
                $scope.gridOptions.data = jobSet.elements;
                
                that.loaded = true;
                that.loading = false;
                
                var width = angular.element(document.getElementsByClassName('grid-wrapper')[0]['clientWidth']);
                var height = angular.element(document.getElementsByClassName('grid-wrapper')[0]['clientHeight']);
                angular.element(document.getElementsByClassName('grid')[0]).css('height', height[0] + 'px');
                angular.element(document.getElementsByClassName('grid')[0]).css('width', width[0] + 'px');
            }, function() {
                that.loading = false;
                console.log("No jobs...");
            });
        }
    }
    
    this.setAutoRefresh = function(refresh) {
        this.autoRefresh = refresh;
    };
    
    this.autoRefresh = $interval(function() {
        if ($scope.currentViewScope != 'service_overview') {
            that.loadJobData();           
        }
    }, 30000);
    
}]);
