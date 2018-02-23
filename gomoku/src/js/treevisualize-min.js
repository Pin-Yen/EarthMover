var D3 = D3 || {};

D3.requestTree = function() {
  var request = new XMLHttpRequest();
  request.open('POST', '/visualize');

  request.onreadystatechange = function() {
    if (request.readyState != 4 || request.status != 200)
      return;

    if (request.responseText) D3.drawTree(JSON.parse(request.responseText));
  }

  // add session id to payload.
  var params = {};
  params.sessionId = sessionManager.getSessionID();
    
  request.send(JSON.stringify(params));
}

D3.removeTree = function() {
  d3.select("#tv-svg").select("g").remove();
}

D3.drawTree = function(treeData) {
  // Assigns parent, children, height, depth.
  // Sort childrens by total simulations in descendant order.
  var root = d3.hierarchy(treeData, function childrenAccessor(d) { return d.ch; })
               .sort(function(child1, child2) {
                 return child2.data.tc - child1.data.tc;
               })

  var margin = { left: 60, top: 40, right: 60, bottom: 40};

  // append the svg object to the body of the page
  // appends a 'group' element to 'svg'
  // moves the 'group' element to the top left margin
  var svg = d3.select("#tv-svg");

  var g = svg.select("g").empty() ?
      svg.append("g").attr("transform", "translate("+ margin.left + "," + margin.top + ")") :
      svg.select("g");

  var idCounter = 0, duration = 600;

  var verticalSpacing, horizontalSpacing;

  update();

  svg.transition().duration(duration)
     .attr("width", root.height * horizontalSpacing + margin.left + margin.right)
     .attr("height", (root.leaves().length - 1) * verticalSpacing + margin.top + margin.bottom);

  function update() {
    var t = d3.transition().duration(duration);

    var maxBranch = parseInt($('#tv-branch-factor').val(), 10),
        minCount = parseInt($('#tv-count-threshold').val(), 10);

    hideChildren(root, maxBranch, minCount);

    function hideChildren(node, branch, count) {
      if (!node.children) return;
      var reserved = 0;
      for (reserved = 0; reserved < branch && reserved < node.children.length; ++reserved) {
        if (node.children[reserved].data.tc < count) {
          break;
        }
      }
      if (reserved < 1) {
        node.children = null;
        return;
      }
      node.children = node.children.slice(0, reserved);
      node.children.forEach(function(d) {
        hideChildren(d, branch, count);
      });
    }

    verticalSpacing = 300 / root.leaves().length + 10;

    var deepest = 0;

    // assigns leaves' x position
    // find the deepest depth
    root.leaves().forEach(function(d, i) {
      d.x = i * verticalSpacing ;
      if (d.depth > deepest) deepest = d.depth;
    });

    horizontalSpacing = 240 / (deepest + 1) + 60;

    // Compute the new tree layout.
    var nodes = root.descendants(),
        links = nodes.slice(1);

    // assings position
    nodes.forEach(function(d) {
      d.x = d.leaves()[0].x;
      d.y = d.depth * horizontalSpacing;
    });

    // ****************** Nodes section ***************************

    // Update the nodes
    // id is a stringfied index serious form current node to root
    var node = g.selectAll('g.node')
        .data(nodes, function(d) {
          if (!d.id) {
            d.id = "id" + d.data.i;
            var currentNode = d;
            while (currentNode = currentNode.parent) {
              d.id += currentNode.data.i;
            }
          }

          return d.id;
        });

    // Enter any new nodes at the parent's previous position.
    // parent's previous position stored in DOM's property (at node.update)
    // and can select by corresponding id
    var nodeEnter = node.enter().append('g')
        .attr('id', function(d) {
          return d.id;
        })
        .attr('class', 'node')
        .attr("transform", function(d) {
          var currentNode = d;

          // continuously finding parent's DOM property to find previous position
          while (currentNode = currentNode.parent) {
            if (d.previousPos = d3.select("#" + currentNode.id).property("previousPos")) {
              break;
            }
          }

          // if previous position undefined,
          // means that either is painting a new tree (root is different) or is at root (no parent)
          // set the pervious position to origin (0, 0)
          if (!d.previousPos) d.previousPos = {x: 0, y: 0};

          return "translate(" + d.previousPos.y + "," + d.previousPos.x + ")";
        });

    // Add Circle for the nodes
    nodeEnter.append('circle')
        .attr('r', 0)
        .style("fill", function(d) {
          return d.data.wt ? "#444" : "#ffe";
        })
        .style("stroke", function(d) {
          return gradient(d.data.wr);
        });

    // Add labels for the nodes
    // labels will be at the right of the node,
    nodeEnter.append('text')
        .attr("x", 12)
        .attr("y", 0)
        .attr("text-anchor", "start")
        .style("font-size", 0)
        .text(function(d) {
          return d.data.i == 225 ? "(pass)" :
              ("(" + String.fromCharCode(65 + d.data.i % 15) + (Math.floor(d.data.i / 15) + 1) + ")");
        })
        .style("fill-opacity", 0);

    nodeEnter.append("line")
        .filter(function(d) { return d.data.wol != 0; })
        .attr("x1", 0)
        .attr("x2", 0)
        .attr("y1", 0)
        .attr("y2", 0);

    // UPDATE
    // update onclick handler
    nodeEnter.merge(node).on('click', click);
    nodeEnter.merge(node).on('mouseover', mouseOver);
    nodeEnter.merge(node).on('mouseout', mouseOut);

    // Transition to the proper position for the node
    var nodeUpdate = nodeEnter.merge(node).transition(t)
        .attr("transform", function(d) {
           return "translate(" + d.y + "," + d.x + ")";
        });

    // Update the node attributes and style
    // using 'each' method to prevent repeating calling width()
    // and storing current position in DOM's property
    nodeUpdate.each(function(d) {
      d3.select(this).property("previousPos", { x: d.x, y: d.y });

      var w = width(d.data.tc);

      var r = w * .75 + "px",
          cw = w * .25 + "px",
          c = gradient(d.data.wr);

      var l = 0,
          nl = 0,
          lw = w * .2 + "px",
          s = null;

      var f = w + "px",
          dy = w * .33 + "px";

      if (d.data.wol != 0) {
        l = w * .44 + "px";
        nl = -w * .44 + "px";
        s = d.data.wol == 1 ? gradient(1) : gradient(0);
      }

      var update = d3.select(this).transition(t);

      update.select('circle')
          .attr('r', r)
          .style("stroke", c)
          .style("stroke-width", cw);

      update.select('line')
          .attr("x1", nl)
          .attr("x2", l)
          .attr("y1", nl)
          .attr("y2", l)
          .style("stroke-width", lw)
          .style("stroke", s);

      update.select('text')
        .attr("dy", dy)
        .style("font-size", f)
        .style("fill-opacity", 1);
    });

    // EXIT
    // Remove any exiting nodes
    // transition to exit position
    var nodeExit = node.exit().transition(t)
        .attr("transform", function(d) {
          var currentNode = d;

          // continuously finding the parent that no exit position
          while (currentNode = currentNode.parent) {
            if (!currentNode.exitPos) {
              currentNode = d3.select("#" + currentNode.id).datum();
              d.exitPos = { x: currentNode.x, y: currentNode.y };
              break;
            }
          }

          // exit position undefined when painting a new tree (root is different)
          // set the exit position to origin (0, 0)
          if (!d.exitPos) d.exitPos = { x: 0, y: 0 };

          return "translate(" + d.exitPos.y + "," + d.exitPos.x + ")";
        })
        .remove();

    // reduce the node circles size to 0
    nodeExit.select('circle')
        .attr('r', 0);

    // reduce the opacity of text labels
    nodeExit.select('text')
        .style('fill-opacity', 0);

    // reduce the node lines size to 0
    nodeExit.select('line')
        .attr("x1", 0)
        .attr("x2", 0)
        .attr("y1", 0)
        .attr("y2", 0);

    // ****************** links section ***************************

    // Update the links...
    var link = g.selectAll('path.link')
        .data(links, function(d) { return d.id; });

    // Enter any new links at the parent's previous position
    var linkEnter = link.enter().insert('path', "g")
        .attr("class", "link")
        .attr('d', function(d) {
          return diagonal(d.previousPos, d.previousPos);
        });

    // UPDATE
    // Transition back to the parent element position
    linkEnter.merge(link).transition(t)
        .attr('d', function(d) { return diagonal(d, d.parent); })
        .style("stroke-width", function(d) {
          return width(d.data.tc) + "px";
        })
        .style("stroke", function(d) {
          return gradient(d.data.wr);
        });

    // Remove any exiting links
    link.exit().transition(t)
        .attr('d', function(d) {
          var c = diagonal(d.exitPos, d.exitPos);
          d.exitPos = null;
          return c;
        })
        .remove();

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
      update();
    }

    function mouseOver(d) {
      updateInf(d);
      var evolve = [];
      while (d.parent) {
        var data = {
          row: Math.floor(d.data.i / 15),
          col: Math.floor(d.data.i % 15),
          color: d.data.wt ? 'black' : 'white'
        }
        evolve.unshift(data);
        d = d.parent;
      }
      if (evolve.length) board.drawEvolve(evolve);
    }

    mouseOut();

    function mouseOut() {
      updateInf(root.children ? root.children[0] : root);
      board.drawAll();
    }

    function updateInf(d) {
      d3.select('#tv-inf-position')
        .text(String.fromCharCode(65 + d.data.i % 15) + (Math.floor(d.data.i / 15) + 1));
      d3.select('#tv-inf-count').text('count: ' + d.data.tc);
      var winRate = d.data.wr;
      d3.select('#tv-inf-win-rate-pointer').style('left', function() {
        return (38 + 120 * winRate) + "px";
      })
      .style('background', function() {
        return gradient(winRate);
      });
      winRate = Math.floor(winRate * 100);
      d3.select('#tv-inf-win-rate-white').text(100 - winRate);
      d3.select('#tv-inf-win-rate-black').text(winRate);
    }

    function gradient(level) {
      var h;
      if (level < (19 / 35)) {
        h = (Math.asin(level * 3.5 - .9) * 180 / Math.PI + 90) * (2 / 3);
      } else {
        h = 120 + (Math.asin(level * 3.5 - 2.9) * 180 / Math.PI + 90) * (2 / 3);
      }

      return `hsl(${h},80%,50%)`;
    }

    function width(count) {
      return Math.pow(count, .24) + 6;
    }
  }
}