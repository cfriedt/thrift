#!/usr/bin/env python3
import sys
import pprint

from thrift import Thrift
from thrift.transport import TTransport, TSocket, TSSLSocket, THttpClient
from thrift.protocol.TBinaryProtocol import TBinaryProtocol, TBinaryProtocolFactory
from thrift.server import TServer

sys.path.append('gen-py')

from st060115 import St060115
from st060115.ttypes import *

host = 'localhost'
port = 9090

class St060115Handler(object):
    def __init__(self):
        pass
    
    def update(self, msg):
        print('received message: {}'.format(msg))

def main(arg):

    handler = St060115Handler()
    processor = St060115.Processor(handler)
    transport = TSocket.TServerSocket(host='localhost', port=9090)
    tfactory = TTransport.TBufferedTransportFactory()
    pfactory = TBinaryProtocolFactory()

    server = TServer.TSimpleServer(processor, transport, tfactory, pfactory)

    server.serve()

if __name__ == '__main__':
    try:
        main(sys.argv)
    except Thrift.TException as tx:
        print('%s' % tx.message)
