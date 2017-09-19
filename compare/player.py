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

    self.blackWinCount = 0
    self.whiteWinCount = 0
    self.sessionCookie = None

  def init(self):
    """
    Request AI to set rule and level and initialize
    """
    conn = client.HTTPConnection('localhost', self.port)
    conn.set_debuglevel(const.DEBUG_CONN)
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
    conn.set_debuglevel(const.DEBUG_CONN)

    if row == None or col == None:
      return

    # pass
    elif row == -1 and col == -1:
      print("INFO: ", "enter /pass ", flush=True)
      conn.request('POST', 'exit /pass', headers={'Cookie' : self.sessionCookie})
      print("INFO:", "exit /pass ", flush=True)

    # play the point, make sure the previous move exists
    elif row >= 0 and col >= 0:
      requestBody = {}
      requestBody['row'] = row
      requestBody['col'] = col
      ## TODO: set cookie in header
      print("INFO: ", "enter /play ", flush=True)
      conn.request('POST', '/play', body=json.dumps(requestBody), headers={'Cookie' : self.sessionCookie})
      print("INFO: ", "exit /play ", flush=True)
    else:
      raise ValueError("(row, col) not valid")


    # Read the response (since python requires us to read the response before making another request),
    # though we don't need the response info.
    print("INFO: ", "before reading response", flush=True)
    response = conn.getresponse()
    print(response.read())
    print("INFO: ", "after reading response")

    print("INFO:", "before closing connection", flush=True)
    conn.close()
    print("INFO:", "after closing connection", flush=True)


    return

  def think(self):
    """
    Request a move from player, 
    returns row, col, status, where row, col is the player's decision, status is the game status.
    """
    conn = client.HTTPConnection('localhost', self.port)
    conn.set_debuglevel(const.DEBUG_CONN)

    print("INFO: ", "enter /think ", flush=True)

    conn.request('POST', '/think', headers={'Cookie' : self.sessionCookie})
    print("INFO: ", "exit /think ", flush=True)

    print("INFO: ", "before reading response", flush=True)
    response = conn.getresponse()
    body = json.load(response)
    print(response.read())
    print("INFO: ", "after reading response")

    print("INFO:", "before closing connection", flush=True)
    conn.close()
    print("INFO:", "after closing connection", flush=True)

    return (body['row'], body['col'], body['winner'])

  def quit(self):
    pass
    # shut down EM session.

  def incrementWin(self, color):
    self.winCount += 1
    if color == const.BLACK:
      self.blackWinCount += 1
    elif color == const.WHITE:
      self.whiteWinCount += 1
    else:
      raise ValueError()

  def incrementLose(self):
    self.loseCount += 1

  def incrementTie(self):
    self.tieCount += 1

