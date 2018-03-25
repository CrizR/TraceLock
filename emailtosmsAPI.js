
//
//
//https://github.com/mscdex/node-imap // for imap
//https://gist.github.com/bergie/1226809 // for imap and mailparser

//https://github.com/typpo/textbelt // textbelt open source

'use strict';

var http = require('http');
var httpServer = http.createServer(function (req, res) {
  res.writeHead(200, {'Content-Type': 'text/plain'});
  res.end('Hello World/n');
});

//var nowjs = require("now");
//var everyone = nowjs.initialize(httpServer);
//everyone.now.serverFunction(openInbox);

httpServer.listen(1337, "127.0.0.1");


var Imap = require('imap');
var inspect = require('util').inspect;

var mailparser = require('mailparser').MailParser;
var parser = new mailparser;

var imap = new Imap({
  user: 'bikelockiot@gmail.com',
  password: 'bikelock1234',
  host: 'imap.gmail.com',
  port: 993,
  tls: true
});

var text = require('textbelt');

var lockbroken = 'ALERT! Your bike lock has been tampered with.';

var location = 'This is the location of your bike...'

var opts = {
  fromAddr: 'bikelockiot@gmail.com',
  fromName: 'BikeLock',
  region: 'us',
  subject: ''
}

function openInbox(cb) {
  imap.openBox('INBOX', true, cb);
}

imap.once('ready', function() {
  openInbox(function(err, box) {
  if (err) throw err;
  var f = imap.seq.fetch(box.messages.total + ':*', { bodies: ['HEADER.FIELDS (FROM)','TEXT'] });
  f.on('message', function(msg, seqno) {
    console.log('Message #%d', seqno);
    var prefix = '(#' + seqno + ') ';
    msg.on('body', function(stream, info) {
      if (info.which === 'TEXT')
        console.log(prefix + 'Body [%s] found, %d total bytes', inspect(info.which), info.size);
      var buffer = '', count = 0;
      stream.on('data', function(chunk) {
        count += chunk.length;
        buffer += chunk.toString('utf8');
        if (info.which === 'TEXT')
          console.log(prefix + 'Body [%s] (%d/%d)', inspect(info.which), count, info.size);
          text.sendText('9734126721', lockbroken, opts, function(err){
            if (err) {
              console.log(err);
            }
          });
          console.log(opts);
          console.log('SMS SENT');
      });
      stream.once('end', function() {
       if (info.which !== 'TEXT')
         console.log(prefix + 'Parsed header: %s', inspect(Imap.parseHeader(buffer)));

     });
   });
    msg.once('attributes', function(attrs) {
      console.log(prefix + 'Attributes: %s', inspect(attrs, false, 8));
    });
    msg.once('end', function() {
      console.log(prefix + 'Finished');
    });
  });
  f.once('error', function(err) {
    console.log('Fetch error: ' + err);
  });
  f.once('end', function() {
    console.log('Done fetching all messages!');
    imap.end();
  });
});
});

imap.once('error', function(err) {
  console.log(err);
});

imap.once('end', function() {
  console.log('Connection ended');
});

imap.connect();