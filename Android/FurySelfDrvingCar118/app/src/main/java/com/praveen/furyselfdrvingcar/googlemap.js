//You can calculate directions (using a variety of methods of transportation) by using the DirectionsService object.
var directionsService = new google.maps.DirectionsService();
//Define a variable with all map points.
var _mapPoints = new Array();
//Define a DirectionsRenderer variable.
var _directionsRenderer = '';


//InitializeMap() function is used to initialize google map on page load.
function InitializeMap() {

    //DirectionsRenderer() is a used to render the direction
    _directionsRenderer = new google.maps.DirectionsRenderer();

    //Set the your own options for map.
    var myOptions = {
        zoom: 6,
        center: new google.maps.LatLng(21.7679, 78.8718),
        mapTypeId: google.maps.MapTypeId.ROADMAP
    };

    //Define the map.
    var map = new google.maps.Map(document.getElementById("dvMap"), myOptions);

    //Set the map for directionsRenderer
    _directionsRenderer.setMap(map);

    //Set different options for DirectionsRenderer mehtods.
    //draggable option will used to drag the route.
    _directionsRenderer.setOptions({
        draggable: true
    });

    //Add the doubel click event to map.
    google.maps.event.addListener(map, "dblclick", function (event) {
        //Check if Avg Speed value is enter.
        if ($("#txtAvgSpeed").val() == '') {
            alert("Please enter the Average Speed (km/hr).");
            $("#txtAvgSpeed").focus();
            return false;
        }

        var _currentPoints = event.latLng;
        _mapPoints.push(_currentPoints);
        getRoutePointsAndWaypoints();
    });

    //Add an event to route direction. This will fire when the direction is changed.
    google.maps.event.addListener(_directionsRenderer, 'directions_changed', function () {
        computeTotalDistanceforRoute(_directionsRenderer.directions);
    });

}
function getRoutePointsAndWaypoints() {
    //Define a variable for waypoints.
    var _waypoints = new Array();

    if (_mapPoints.length > 2) //Waypoints will be come.
    {
        for (var j = 1; j < _mapPoints.length - 1; j++) {
            var address = _mapPoints[j];
            if (address !== "") {
                _waypoints.push({
                    location: address,
                    stopover: true  //stopover is used to show marker on map for waypoints
                });
            }
        }
        //Call a drawRoute() function
        drawRoute(_mapPoints[0], _mapPoints[_mapPoints.length - 1], _waypoints);
    } else if (_mapPoints.length > 1) {
        //Call a drawRoute() function only for start and end locations
        drawRoute(_mapPoints[_mapPoints.length - 2], _mapPoints[_mapPoints.length - 1], _waypoints);
    } else {
        //Call a drawRoute() function only for one point as start and end locations.
        drawRoute(_mapPoints[_mapPoints.length - 1], _mapPoints[_mapPoints.length - 1], _waypoints);
    }
}
//drawRoute() will help actual draw the route on map.
function drawRoute(originAddress, destinationAddress, _waypoints) {
    //Define a request variable for route .
    var _request = '';

    //This is for more then two locatins
    if (_waypoints.length > 0) {
        _request = {
            origin: originAddress,
            destination: destinationAddress,
            waypoints: _waypoints, //an array of waypoints
            optimizeWaypoints: true, //set to true if you want google to determine the shortest route or false to use the order specified.
            travelMode: google.maps.DirectionsTravelMode.DRIVING
        };
    } else {
        //This is for one or two locations. Here noway point is used.
        _request = {
            origin: originAddress,
            destination: destinationAddress,
            travelMode: google.maps.DirectionsTravelMode.DRIVING
        };
    }

    //This will take the request and draw the route and return response and status as output
    directionsService.route(_request, function (_response, _status) {
        if (_status == google.maps.DirectionsStatus.OK) {
            _directionsRenderer.setDirections(_response);
        }
    });
}
