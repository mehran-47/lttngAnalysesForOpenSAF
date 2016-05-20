"use strict"

//Global variable to hold plot data.
var SIstoplot = {
	'data_arrays':{}
}

window.onload = function(){	
	getCallWrapper();
	var intervalExec = setInterval(getCallWrapper, 3000);
	var canvas = document.getElementById("plotCanvas");
	var context = canvas.getContext("2d");
	drawPlotWithContext(context, SIstoplot);
	var drawWrapper = function(){		
		//alert(JSON.stringify(SIstoplot.data_arrays[anSIload]));
		//testPlot.innerHTML = JSON.stringify(x_y);
		drawPlotWithContext(context, SIstoplot);
	}
	var dummIntervalExec = setInterval(drawWrapper, 3000);
};


function drawPlotWithContext(context, objectsToPlot){
	var maxY = 350;
	var maxX = 1024;
	var scaleX = 10;
	var rgb = [128,128,128];
	var objCount = 0;
	var canvas = document.getElementById("plotCanvas");
	context.clearRect(0,0, canvas.width, canvas.height);
	for(var anSIload in objectsToPlot.data_arrays){
		objCount++;
		if(objectsToPlot.data_arrays[anSIload]['y'].length*scaleX > maxX){	
			objectsToPlot.data_arrays[anSIload]['y'].shift();
			objectsToPlot.data_arrays[anSIload]['x'].pop();
		}
		context.beginPath();
		for(var i=0; i<objectsToPlot.data_arrays[anSIload]['y'].length-1; i++){
			context.moveTo(objectsToPlot.data_arrays[anSIload]['x'][i]*scaleX,  maxY-maxY*objectsToPlot.data_arrays[anSIload]['y'][i]/100 );
			context.lineTo(objectsToPlot.data_arrays[anSIload]['x'][i+1]*scaleX,  maxY-maxY*objectsToPlot.data_arrays[anSIload]['y'][i+1]/100 );			
		}
		context.lineWidth = 2;
		//context.strokeStyle = objCount%2==1 ? '#207ce5' : '#e73827';
		context.strokeStyle = anSIload == 'safSi=AmfDemo,safApp=AmfDemo1' ? '#207ce5' : '#e73827'
		context.stroke();
	//context.strokeStyle = '#ff0000';    
	}	
};


function getCallWrapper(){
	get_information('guiconfig.json', function(data){ 
			var parsedConfig = JSON.parse(data);
			get_information("http://"+parsedConfig.ip+":"+parsedConfig.port , JSON_to_HTML);
		}
	);
}

function get_information(link, callback) {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", link, true);
    xhr.withCredentials = true;
    xhr.onreadystatechange = function(){
        if (xhr.readyState === 4) {
            callback(xhr.responseText);
        }
    };
    xhr.send(null);
};

function createAndAdd(elementType, parent, cssClass, HTMLstring, elementId){
	var elementToAdd = document.createElement(elementType);
	elementToAdd.className = cssClass;
	if(typeof elementId != 'undefined'){
		elementToAdd.id = elementId;
	}
	elementToAdd.innerHTML = HTMLstring;
	parent.appendChild(elementToAdd);
}

function JSON_to_HTML(text){
	var data = JSON.parse(text);
	var parentDiv = document.getElementById('architecture_overview');
	parentDiv.innerHTML = '';
	//alert(JSON.stringify(completePull.summ));
	var SIcount = 0;
	var nodeCount = 0;
	for(var anSI in data.SIs){
		SIcount++;
		var SI_div = document.createElement('div');
		var load_div = document.createElement('div');
		var loadString = "";
		SI_div.className = 'conceptualContainer SIclass';
		load_div.className = 'conceptualContainer SIload';
		SI_div.id = 'SI_' + SIcount.toString();
		SI_div.innerHTML = '<p>'+anSI+'</p>';
		for(var aMetric in data.summ[anSI]){
			var aLoadLine = document.createElement('div');
			aLoadLine.innerHTML += aMetric + ' : ' + data.summ[anSI][aMetric];
			load_div.appendChild(aLoadLine);
			if(aMetric=='cpu_usage'){
				if(!(anSI in SIstoplot.data_arrays)){
					SIstoplot.data_arrays[anSI] = {'x':[], 'y':[] };
				}else{
					SIstoplot.data_arrays[anSI]['y'].push(data.summ[anSI]['cpu_usage']);
					SIstoplot.data_arrays[anSI]['x'].push(SIstoplot.data_arrays[anSI]['y'].length-1);
				}
			}
		}
		SI_div.appendChild(load_div);
		createAndAdd('div', SI_div, 'clearFloat', '');
		for(var aNode in data.SIs[anSI]){
			nodeCount++;
			var SU_name = "";
			var node_div = document.createElement('div');
			var details_div = document.createElement('div');
			node_div.id = 'node_' + nodeCount;
			node_div.className = 'conceptualContainer nodeStyle';
			//node_div.style.width = 96/Object.keys(data.SIs[anSI]).length + '%';
			details_div.className = 'detailsToolTip';
			for(var endObj in JSON.stringify(data.SIs[anSI][aNode]).split("\}")){
				details_div.innerHTML += '}<br/>'+endObj
			}
			details_div.innerHTML = JSON.stringify(data.SIs[anSI][aNode]);
			if("Active" in data.SIs[anSI][aNode]){
				if (Object.keys(data.SIs[anSI][aNode]["Active"]).length!=0){
					node_div.className += ' active';
					for(var CSI_name in  data.SIs[anSI][aNode]["Active"]){
						for(var component_name in data.SIs[anSI][aNode]["Active"][CSI_name]){
							//alert(component_name.split(',safSu\=')[1]);
							SU_name = 'safSU='+ component_name.split(',safSu\=')[1].toString();
							break;
						}
						break;
					}
				}
			}
			if("Standby" in data.SIs[anSI][aNode]){
				if (Object.keys(data.SIs[anSI][aNode]["Standby"]).length!=0){
					node_div.className += ' standby';
					for(var CSI_name in  data.SIs[anSI][aNode]["Standby"]){
						for(var component_name in data.SIs[anSI][aNode]["Standby"][CSI_name]){
							SU_name = 'safSU='+ component_name.split(',safSu\=')[1].toString();
							break;
						}
						break;
					}
				}
			}
			node_div.innerHTML = '<b>'+aNode+'</b><p>'+SU_name+'</p>';
			node_div.appendChild(details_div);
			SI_div.appendChild(node_div);
		}
		parentDiv.appendChild(SI_div);
	}
};
