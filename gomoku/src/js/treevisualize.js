var D3 = D3 || {};

D3.requestTree = function() {
  var request = new XMLHttpRequest();
  request.open('POST', '/visualize');

  request.onreadystatechange = function() {
    if (request.readyState != 4 || request.status != 200)
      return;

    D3.drawTree(JSON.parse(request.responseText));
  }

  request.send("requesting mct");
}

D3.removeTree = function() {
  d3.select("#tree-visualize").select("svg").select("g").remove();
}

D3.drawTree = function(treeData) {
  // Assigns parent, children, height, depth.
  // Sort childrens by total simulations in descendant order.
  var root = d3.hierarchy(treeData)
               .sort(function(child1, child2) {
                 return child2.data.totalCount - child1.data.totalCount;
               });

  var margin = { left: 80, top: 20, right: 80, bottom: 20};

  // append the svg object to the body of the page
  // appends a 'group' element to 'svg'
  // moves the 'group' element to the top left margin
  var svg = d3.select("#tree-visualize").select("svg")
      .attr("width", root.height * 80 + margin.left + margin.right)
      .attr("height", (root.leaves().length - 1) * 20 + margin.top + margin.bottom);

  var svg = svg.select("g").empty() ?
      svg.append("g").attr("transform", "translate("+ margin.left + "," + margin.top + ")") :
      svg.select("g");

  root.x0 = 0;
  root.y0 = 0;

  var idCounter = 0, duration = 750;

  update(root);

  function update(source) {
    // assigns leaves' x position
    var leavesCounter = -1;
    root.leaves().forEach(function(d){ d.x = ++leavesCounter * 20});

    // Compute the new tree layout.
    var nodes = root.descendants(),
        links = root.descendants().slice(1);

    // assings position
    nodes.forEach(function(d) {
      d.x = d.leaves()[0].x;
      d.y = d.depth * 80;
    });

    // ****************** Nodes section ***************************

    // Update the nodes...
    var node = svg.selectAll('g.node')
        .data(nodes, function(d) {return d.id || (d.id = ++idCounter); });

    // Enter any new nodes at the parent's previous position.
    var nodeEnter = node.enter().append('g')
        .attr('class', 'node')
        .attr("transform", function(d) {
          return "translate(" + source.y0 + "," + source.x0 + ")";
        });

    // Add Circle for the nodes
    nodeEnter.append('circle')
        .attr('r', 0)
        .style("fill", function(d) {
          return d.data.whiteTurn ? "black" : "white";
        })
        .style("stroke", function(d) {
          return gradient(d.data.winRate);
        });

    // Add labels for the nodes
    // labels will be at the right of the node,
    nodeEnter.append('text')
        .attr("dy", ".35em")
        .attr("x", 15)
        .attr("y", 0)
        .attr("text-anchor", "start")
        .style("fill-opacity", 0);

    nodeEnter.append("line")
        .filter(function(d) { return d.data.isWinning || d.data.isLosing; })
        .attr("x1", 0)
        .attr("x2", 0)
        .attr("y1", 0)
        .attr("y2", 0);

    // UPDATE
    // Transition to the proper position for the node
    nodeEnter.merge(node).on('click', click);

    var nodeUpdate = nodeEnter.merge(node).transition()
        .duration(duration)
        .attr("transform", function(d) {
           return "translate(" + d.y + "," + d.x + ")";
        })

    // Update the node attributes and style
    nodeUpdate.select('circle')
        .attr('r', function(d) {
           return stroke(d) * (3 / 4) + "px";
        })
        .style("fill", function(d) {
           return d.data.whiteTurn ? "black" : "white";
        })
        .style("stroke", function(d) {
          return gradient(d.data.winRate);
        })
        .style("stroke-width", function(d) {
          return stroke(d) * (1 / 4) + "px";
        })
        .attr('cursor', 'pointer');

    nodeUpdate.select('line')
        .filter(function(d) { return d.data.isWinning || d.data.isLosing; })
        .attr("x1", function(d) {
          return -stroke(d) * (4 / 9) + "px";
        })
        .attr("x2", function(d) {
          return stroke(d) * (4 / 9) + "px";
        })
        .attr("y1", function(d) {
          return -stroke(d) * (4 / 9) + "px";
        })
        .attr("y2", function(d) {
          return stroke(d) * (4 / 9) + "px";
        })
        .style("stroke-width", function(d){
        // set the link's stroke-width according to total simulation count.
          return stroke(d) / 5 + "px";
        })
        .style("stroke", function(d) {
          return d.data.isWinning ? gradient(100) : gradient(0);
        });

    nodeUpdate.select('line')
        .filter(function(d) { return !(d.data.isWinning || d.data.isLosing); })
        .attr("x1", 0)
        .attr("x2", 0)
        .attr("y1", 0)
        .attr("y2", 0)
        .style("stroke-width", null)
        .style("stroke", null);

    nodeUpdate.select('text')
        .text(function(d) {
          return "(" + String.fromCharCode(65 + d.data.c) + (d.data.r + 1) + ")";
        })
        .style("fill-opacity", 1);

    // Remove any exiting nodes
    var nodeExit = node.exit().transition()
        .duration(duration)
        .attr("transform", function(d) {
          return "translate(" + source.y + "," + source.x + ")";
        })
        .remove();

    // On exit reduce the node circles size to 0
    nodeExit.select('circle')
        .attr('r', 0);

    // On exit reduce the opacity of text labels
    nodeExit.select('text')
        .style('fill-opacity', 0);

    nodeExit.select('line')
        .attr("x1", 0)
        .attr("x2", 0)
        .attr("y1", 0)
        .attr("y2", 0);

    // ****************** links section ***************************

    // Update the links...
    var link = svg.selectAll('path.link')
        .data(links, function(d) { return d.id; });

    // Enter any new links at the parent's previous position.
    var linkEnter = link.enter().insert('path', "g")
        .attr("class", "link")
        .attr('d', function(d){
          var o = {x: source.x0, y: source.y0};
          return diagonal(o, o);
        });

    // UPDATE
    // Transition back to the parent element position
    linkEnter.merge(link).transition()
        .duration(duration)
        .attr('d', function(d){ return diagonal(d, d.parent); })
        .style("stroke-width", function(d) {
          // set the link's stroke-width according to total simulation count.
          return stroke(d) + "px";
        })
        .style("stroke", function(d) {
          return gradient(d.data.winRate);
        });

    // Remove any exiting links
    link.exit().transition()
        .duration(duration)
        .attr('d', function(d) {
          var o = {x: source.x, y: source.y};
          return diagonal(o, o);
        })
        .remove();

    // Store the old positions for transition.
    nodes.forEach(function(d) {
      d.x0 = d.x;
      d.y0 = d.y;
    });

    // Creates a curved (diagonal) path from parent to the child nodes
    function diagonal(s, d) {

      path = `M ${s.y} ${s.x}
      C ${(s.y + d.y) / 2} ${s.x},
      ${(s.y + d.y) / 2} ${d.x},
      ${d.y} ${d.x}`;

      return path;
    }

    // Toggle children on click.
    function click(d) {
      if (d.children) {
        d.hiddenChildren = d.children;
        d.children = null;
      } else {
        d.children = d.hiddenChildren;
        d.hiddenChildren = null;
      }
      update(d);
    }

    function gradient(level) {
      var hStart = 0 // red
      var hEnd = 200 //blue

      h = hStart + (hEnd - hStart) * level;
      return `hsl(${h},100%,45%)`;
    }

    function stroke(d) {
      return Math.pow(d.data.totalCount, .25) + 5;
    }
  }
}