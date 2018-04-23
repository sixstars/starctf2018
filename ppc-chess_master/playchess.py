import chess,random
import string
from sys import argv
from hashlib import sha256
from SocketServer import ThreadingTCPServer,BaseRequestHandler,socket


def getMove(board,rh):
	
	while(True):
		try:
			rh.request.send("\ninput your move(like e2e4):\n")		
			s=rh.request.recv(5)
			
			move=chess.Move.from_uci(s[:-1])
			print move			
			if move in board.legal_moves:
				return move
			else:
				rh.request.send( "ilegal move\n")
		except :
			
			rh.request.send("illegal input\n")


def evaluateBoard(board):
	score=0	
	s=str(board)
	score+=(s.count("p")-s.count("P"))*1
	score+=(s.count("n")-s.count("N"))*3
	score+=(s.count("b")-s.count("B"))*3
	score+=(s.count("r")-s.count("R"))*5
	score+=(s.count("q")-s.count("Q"))*9
	score+=(s.count("k")-s.count("K"))*30
	
	return score		
def aiplay(board):
	moves=list(board.legal_moves)
	bestmove=[]
	bestvalue=-9999
	for m in moves:
		board.push(m)	
		v=evaluateBoard(board)
		board.pop()
		if(v==bestvalue):
			bestmove.append(m)
			
		if(v>bestvalue):
			bestmove=[m]
			bestvalue=v
	l=len(bestmove)	

	return bestmove[random.randint(0,l-1)]

def play(rh):
	rh.request.send("game starts\n")
	board=chess.Board()
	while(True):	
		rh.request.send( str(board))
	#human to play	
		board.push(getMove(board,rh))
		if(board.is_checkmate()):
			rh.request.send("you win\n")	
			return
		board.push(aiplay(board))
		if(board.is_checkmate()):
			rh.request.send("you fail\n")			
			exit(0)	
		


class chessHandler(BaseRequestHandler):
	def pofw(self):
		proof="".join([random.choice(string.ascii_letters+string.digits) for i in xrange(20)])
		digest = sha256(proof).hexdigest()
		self.request.send("sha256(xxxx+%s) ==%s\n"%(proof[4:],digest))
		self.request.send("Give me xxxx:")
		x=self.request.recv(4)
		if(len(x)!=4 or sha256(x+proof[4:]).hexdigest()!=digest):
			return False
		return True
	
	def handle(self):
		try:
			self.core_handle()
		except socket.error as e:
			print e
	def core_handle(self):
		if not self.pofw():
			return
		self.request.send(" #  #    ###   #####  #####\n")
		self.request.send("  ##    #   #    #    #\n")
		self.request.send("######  #        #    ###\n")
		self.request.send("  ##    #   #    #    #\n")
		self.request.send(" #  #    ###     #    #\n")
		self.request.send("\n")
		self.request.send("welcome to *ctf chess game\n")
		self.request.send("beat the AI twenty times to get the flag\n")
		self.request.send( "you can input a UCI string(e.g. e2e4) to move. Are you ready?\n")
		self.request.send("Press enter key to continue:\n")
		self.request.recv(1)
		i=0
		while(i<20):	
			play(self)
			i+=1

		self.request.send("YOU ARE CHESS MASTER!The flag is *ctf{chess_is_s0_ea5y}")



if(__name__ =="__main__"):
	ThreadingTCPServer.allow_reuse_address=True
	if(len(argv)<3):
		print "Usage: {} <IP> <Port>".format(argv[0])
	else:
		ip = argv[1]
		port = int(argv[2])
		s=ThreadingTCPServer((ip,port),chessHandler)
		try:
			s.serve_forever()
		except KeyboardInterrupt:
			print "shut down!"
			s.shutdown()
			s.socket.close()
