function requestTree() {

  var  request = new XMLHttpRequest();
  request.open('POST', '/visualize');

  request.onreadystatechange = function() {
    if (request.readyState != 4 || request.status != 200)
      return;

    drawTree(JSON.parse(request.responseText));
  }

  request.send("requesting mct");
}

function removeTree() {
  d3.select("#tree-visualize svg").remove();
}

function drawTree(treeData) {
  // Set the dimensions and margins of the diagram
  var margin = {top: 20, right: 90, bottom: 30, left: 90},
  width = 1400 - margin.left - margin.right,
  height = 800 - margin.top - margin.bottom;

  // append the svg object to the body of the page
  // appends a 'group' element to 'svg'
  // moves the 'group' element to the top left margin
  var svg = d3.select("#tree-visualize").append("svg")
  .attr("width", width + margin.right + margin.left)
  .attr("height", height + margin.top + margin.bottom)
  .append("g")
  .attr("transform", "translate("+ margin.left + "," + margin.top + ")");

  var idCounter = 0, duration = 750, root;

  // declares a tree layout and assigns the size
  var mctsTree = d3.tree().size([height, width]);

  // Assigns parent, children, height, depth.
  // Sort childrens by total simulations in descendant order.
  root = d3.hierarchy(treeData, function(d) { return d.children; })
           .sort(function(child1, child2) {
  return child2.data.totalCount - child1.data.totalCount;
  });

  root.x0 = height / 2;
  root.y0 = 0;

  update(root);


  function update(source) {
    // Assigns the x and y position for the nodes
    var treeData = mctsTree(root);

    // Compute the new tree layout.
    var nodes = treeData.descendants(),
    links = treeData.descendants().slice(1);

    // Normalize for fixed-depth.
    nodes.forEach(function(d){ d.y = d.depth * 100});

    // align to top
    nodes.forEach(function(d) {
      var descendants = d.descendants();
      for (var i = 0; i < descendants.length; ++i) {
        if (descendants[i].x < d.x)
          d.x = descendants[i].x;
      }
    });

    // adjust node sequence
    nodes.forEach(function(d) {
      if (d.children == undefined)
        return;
      for (var i = 0; i < d.children.length; ++i) {
        for (var k = 0; k < d.children.length; ++k) {

          if (d.children[i].data.totalCount < d.children[k].data.totalCount &&
              d.children[i].x < d.children[k].x) {
            // if the order of child i & child k is wrong, swap them.
            var dispacement = d.children[k].x - d.children[i].x;

            var upperChildDescendants = d.children[k].descendants();
            for (var m = 0; m < upperChildDescendants.length; ++m) {
              upperChildDescendants[m].x -= dispacement;
            }

            var lowerChildDescendants = d.children[i].descendants();
            for (var m = 0; m < lowerChildDescendants.length; ++m) {
              lowerChildDescendants[m].x += dispacement;
            }
          }
        }
      }
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
        })
        .on('click', click);

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
    // labels of a leaf node will be at the right of the node,
    // otherwise, it will be above the node.
    nodeEnter.append('text')
        .attr("dy", ".35em")
        .attr("x", function(d) {
          return d.children || d.hiddenChildren ? 0 : 13;
        })
        .attr("y", function(d) {
          return d.children || d.hiddenChildren ? -15 : 0;
        })
        .attr("text-anchor", function(d) {
          return d.children || d._children ? "middle" : "start";
        })
        .text(function(d) { return "(" + String.fromCharCode(65 + d.data.c) + (d.data.r + 1) + ")";
        })
        .style("fill-opacity", 0);


    nodeEnter.append("line")
        .filter(function(d) { return d.data.isWinning || d.data.isLosing; })
        .attr("x1", 0)
        .attr("x2", 0)
        .attr("y1", 0)
        .attr("y2", 0)
        .style("stroke-width", function(d){
        // set the link's stroke-width according to total simulation count.
          return stroke(d) / 5 + "px";
        })
        .style("stroke", function(d) {
          return d.data.isWinning ? gradient(100) : gradient(0);
        });

    // UPDATE
    // Transition to the proper position for the node
    var nodeUpdate = nodeEnter.merge(node).transition()
        .duration(duration)
        .attr("transform", function(d) {
           return "translate(" + d.y + "," + d.x + ")";
        });

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
        });

    nodeUpdate.select('text') .style("fill-opacity", 1);

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
        })
        .style("stroke-width", function(d){
          // set the link's stroke-width according to total simulation count.
          return stroke(d) + "px";
        })
        .style("stroke", function(d) {
          return gradient(d.data.winRate);
        });

    // UPDATE
    var linkUpdate = linkEnter.merge(link);

    // Transition back to the parent element position
    linkUpdate.transition()
        .duration(duration)
        .attr('d', function(d){ return diagonal(d, d.parent); });

    // Remove any exiting links
    var linkExit = link.exit().transition()
        .duration(duration)
        .attr('d', function(d) {
          var o = {x: source.x, y: source.y};
          return diagonal(o, o);
        })
        .remove();

    // Store the old positions for transition.
    nodes.forEach(function(d){
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