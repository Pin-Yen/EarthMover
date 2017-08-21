import http.client as client
import json as json
import const as const

class Player(object):
  """docstring for Player"""
  def __init__(self, port, rule, level):
    self.port = port
    self.rule = rule
    self.level = level

    self.winCount = 0
    self.loseCount = 0
    self.tieCount = 0
    self.sessionCookie = None

    self.init()


  def init(self):
    """
    Request AI to set rule and level and initialize
    """
    conn = client.HTTPConnection('localhost', self.port)
    conn.set_debuglevel(2)
    requestBody = {'level' : self.level, 'rule' : self.rule }

    if self.sessionCookie != None:
      conn.request('POST', '/start', body=json.dumps(requestBody), headers={'Cookie' : self.sessionCookie})
    else:
      conn.request('POST', '/start', body=json.dumps(requestBody))

    response = conn.getresponse()
    response.read()
    self.sessionCookie = response.getheader('Set-Cookie')
    print ('session-cookie', self.sessionCookie)

    conn.close()


  def play(self, row, col):
    """
    Post the previous move (row, col) to player.
    """
    conn = client.HTTPConnection('localhost', self.port)
    conn.set_debuglevel(2)

    if row == None or col == None:
      return

    # pass
    elif row == -1 and col == -1:
      conn.request('POST', '/pass', headers={'Cookie' : self.sessionCookie})

    # play the point, make sure the previous move exists
    elif row >= 0 and col >= 0:
      requestBody = {}
      requestBody['row'] = row
      requestBody['col'] = col
      ## TODO: set cookie in header
      conn.request('POST', '/play', body=json.dumps(requestBody), headers={'Cookie' : self.sessionCookie})
    else:
      raise ValueError("(row, col) not valid")


    # Read the response (since python requires us to read the response before making another request),
    # though we don't need the response info.
    conn.getresponse()
    conn.close()

    return

  def think(self):
    """
    Request a move from player, 
    returns row, col, status, where row, col is the player's decision, status is the game status.
    """
    conn = client.HTTPConnection('localhost', self.port)
    conn.set_debuglevel(2)
    conn.request('POST', '/think', headers={'Cookie' : self.sessionCookie})

    response = conn.getresponse()
    body = json.load(response)

    conn.close()

    return (body['row'], body['col'], body['winner'])

  def quit(self):
    pass
    # shut down EM session.

  def incrementWin(self):
    self.winCount += 1

  def incrementLose(self):
    self.loseCount += 1

  def incrementTie(self):
    self.tieCount += 1

