const width = 790;
const height = 790;
const nodeRadius = 35;
const anchorRadius = 5;

// node type
class Node {
  constructor(label, x, y, angles=[]) {
    this.type = "Node";
    this.x = x;
    this.y = y;
    this.r = nodeRadius;
    this.label = label;

    var self = this; // this is too weird
    this.anchors = [];
    angles.forEach(function(a) { self.anchors.push( new AnchorNode(self, a) ); } );
  }
}

// fixed node anchor point type
class AnchorNode {
  constructor(owner, angle) {
    this.type = "AnchorNode";
    this.owner = owner;
    this.angle = angle;
    this.vx = 0;
    this.vy = 0;
    this.r = 3;
    this.localx = owner.r * Math.cos(this.angle/360*2*Math.PI);
    this.localy = - this.owner.r * Math.sin(this.angle/360*2*Math.PI); // svg inverted y-axis

    this.selected = false;
  }
  get x() { return this.owner.x + this.localx; }
  set x(value) { /* empty:)) */ }
  get y() { return this.owner.y + this.localy; }
  set y(value) { /* empty:)) */ }
}

// path helper node type
class PathAnchor {
  constructor(x, y) {
    this.x = x;
    this.y = y;
    this.vx = 0;
    this.vy = 0;
  }
}
const anchSpace = 20;

// link data type
class Link {
  constructor(d1, d2) {
    this.d1 = d1;
    this.d2 = d2;
    this.pathAnchors = [];

    let distx = Math.abs(d1.x - d2.x);
    let disty = Math.abs(d1.y - d2.y);
    let len = Math.sqrt( distx*distx + disty*disty );
    console.log(len);
    let xScale = d3.scaleLinear()
      .domain([0, len])
      .range([d1.x, d2.x]);
    let yScale = d3.scaleLinear()
      .domain([0, len])
      .range([d1.y, d2.y]);
    let na = Math.floor(len/anchSpace) - 1;
    for (let i=1; i <= na; i++) {
      this.pathAnchors.push( new PathAnchor(xScale(i*anchSpace), yScale(i*anchSpace)) );
    }

    console.log("link      - ", this.nodeNames());
    console.log("  anchors - ", this.pathAnchors);
  }
  length() {
    let dx = d2.x - d1.x;
    let dy = d2.y - d1.y;
    return Math.sqrt(dx*dx + dy*dy);
  }
  nodeNames() {
    return [this.d1.owner.label, this.d2.owner.label];
  }
}

let color = d3.scaleOrdinal()
  .range(d3.schemeCategory20);

class GraphDraw {
  constructor() {
    this.nodes = [];
    this.color = d3.scaleOrdinal().range(d3.schemeCategory20);
    this.svg = d3.select('#svg_container')
      .append('svg')
      .attr('width', width)
      .attr('height', height)
    this.svg
      .on("click", function() {
        let m = d3.mouse(this)
        let svg_x = m[0];
        let svg_y = m[1];
        clickSvg(svg_x, svg_y);
        } )
    this.collideSim = d3.forceSimulation(this.nodes)
        .force("collide",
          d3.forceCollide(nodeRadius + 3)
          .iterations(4))
        .force("centering",
          d3.forceCenter(width/2, height/2))
        .on("tick", this.drawNodes)
        .restart();
    this.draggable = null;

    // root nodes for various item types
    this.nodeGroup = this.svg.append("g");
    this.linkGroup = this.svg.append("g");

    // anchor activation
    this.selectedAnchor = null;
    this.links = [];

    // pythonicism
    self = this;
  }

  addNode(label, x, y, angles) {
    self.addNode2( new Node(label, x, y, angles) );
  }
  addNode2(node) {
    self.nodes.push(node);
    self.resetChargeSim();
    self.drawNodes();
  }
  resetChargeSim() {
    self.distanceSim = d3.forceSimulation(self.nodes)
      .force("repulsion",
          d3.forceManyBody()
          .strength( -40 )
          .distanceMin(20)
          .distanceMax(100))
      .on("tick", self.drawNodes)
      .stop();
  }
  restartChargeSim() {
    self.distanceSim.stop();
    self.distanceSim.alpha(1).restart();
  }
  restartCollideSim() {
    self.collideSim.stop();
    self.collideSim.nodes(self.nodes);
    self.collideSim.alpha(1).restart();
  }
  dragged(d) {
    // reheating is needed during long drags
    if (self.collideSim.alpha() < 0.1) { self.restartCollideSim(); }

    d.x += d3.event.dx;
    d.y += d3.event.dy;
    self.drawNodes();
  }
  dragstarted(d) {
    d.active = true;
    self.restartCollideSim();
  }
  dragended(d) {
    d.active = false;
    self.drawNodes();
    self.restartChargeSim();
  }
  anchorClick(d, i) {
    console.log(i);
    let s = self.selectedAnchor;
    if (s == null) {
      self.selectedAnchor = d;
      d.selected = true;
    }
    else {
      self.links.push(new Link(s, d));
      self.selectedAnchor = null;
      d.selected = false;
    }
    console.log("anchor click - ", d.owner.label, d.x, d.y);
    self.drawNodes();
  }
  drawNodes() {
    // clear all nodes
    if (self.draggable) self.draggable.remove();

    // draw all nodes
    self.draggable = self.nodeGroup.selectAll("g")
      .data(self.nodes)
      .enter()
      .append("g")
      .attr("transform", function(d) { return "translate(" + d.x + "," + d.y + ")"; } )
      .call( d3.drag()
        .on("start", self.dragstarted)
        .on("drag", self.dragged)
        .on("end", self.dragended)
      );

    // draw anchor nodes
    self.draggable.each( function(d, i) {
      let anchors = d3.select(this)
      .append("g")
      .selectAll("circle")
      .data(d.anchors)
      .enter()
      .append("circle")
      .attr('r', anchorRadius)
      .attr("transform", function(p) { return "translate(" + p.localx + "," + p.localy + ")" } )
      .style("fill", "white")
      .style("stroke", "#000")
      .on("mousedown", function(p) {
        // this will prevent the drag behavior from activating, cancelling click events
        d3.event.stopPropagation();
        d3.event.preventDefault();
      } )
      .on("click", self.anchorClick);
    } );

    self.draggable.append('text')
      .text( function(d) { return d.label } )
      .attr("font-family", "sans-serif")
      .attr("font-size", "20px")
      .attr("fill", "white")
      .attr("text-anchor", "middle")
      .attr("dominant-baseline", "middle")
      .lower();

    self.draggable.append('circle')
      .attr('r', function(d) { return d.r; })
      .style("fill", function(d, i) { return color(i); })
      .classed("active", function(d) { return d.active; })
      //.on("mousedown", function(p) {
      //  console.log("circle mousedown: " + p.localx, p.localy)
      //  d3.event.stopPropagation();
      //  d3.event.preventDefault();
      //} )
      //.on("click", function() { console.log("circle click!"); } )
      .lower();

    // now draw all links separately
    if (self.lines) self.lines.remove();
    self.lines = self.linkGroup.selectAll("line")
      .data(self.links)
      .enter()
      .append("line")
      .style("stroke", "black")
      .attr("x1", function(d) { return d.d1.x; })
      .attr("y1", function(d) { return d.d1.y; })
      .attr("x2", function(d) { return d.d2.x; })
      .attr("y2", function(d) { return d.d2.y; })
      .lower();

    /*
    let pathAnchors = []
    self.links.forEach(function(a) {
      a.pathAnchors.forEach( function(b) {
        pathAnchors.push(b)
      ;});
    });

    //console.log(pathAnchors);
    if (self.pathAnchors) self.pathAnchors.remove();
    self.pathAnchors = self.linkGroup.selectAll("circle")
      .data(pathAnchors)
      .enter()
      .append("circle")
      .attr("cx", function(d) { return d.x; })
      .attr("cy", function(d) { return d.y; })
      .attr("r", 5)
      .attr("fill", "black");
    */
  }
}

let gd = null;
// entry point
function run() {
  gd = new GraphDraw();

  // example nodes
  gd.addNode("gauss", 100, 140, [70, 90, 110, 270]);
  gd.addNode("en1", 520, 240, [100]);
  gd.addNode("pg", 450, 44, [270]);
}

// ui interaction
let nodeLabel = '';
let anchArray = [];
let labelHistory = []
let clearTbxCB = null;
function pushNodeLabel(label, anchArr) {
  nodeLabel = label;
  anchorArray = anchArr;
  console.log(anchorArray);
}
function clickSvg(x, y) {
  label = nodeLabel;
  nodeLabel = '';
  anchs = anchorArray;
  anchArray = [];
  if (label != '') {
    gd.addNode(label, x, y, anchs);
    gd.drawNodes();
    if (clearTbxCB) {
      clearTbxCB();
    }
    labelHistory.push(label);
    console.log("a node with label '" + label + "' was added")
  }
}
