# Literography
## A literary cinematography tool

This is a simple generator that takes
an input file (which should be some
sort of story, or literary work with
formatting specifiers) and adds
"Literographic effects" like timed
letter reveal, italics, and other such
things. To have more freedom as a writer.

We shall begin with a basic speed writing
system. This can be done with the following
javascript:

```javascript
const work = document.getElementById("Work");
var speed = 30;
const sleep = ms => new Promise(r => setInterval(r, ms));
const dprint = async msg => { for(let i=0; i<msg.length; i++) {await sleep(speed); work.innerHTML += msg[i] } };
```

where `dprint(msg)` will print `msg` by 1
character per 30ms.

A program could then be written by taking
in the input string and calling:

```javascript
const main = async () => {
    await dprint(program_string);
}
main();
```

This of course forgets about pauses and
stylisation. Pauses are the first and
easier of the two to implement. We can
parse our program by line and if the line
begins with `]` then we will check the
line as though it was a command.

The following script:

```lig
]speed 30
]pause 1000
Hello world
```

should compile to the following:

```javascript

const work = document.getElementById("Work");
var speed = 30;
const sleep = ms => new Promise(r => setInterval(r, ms));
const dprint = async msg => { for(let i=0; i<msg.length; i++) {await sleep(speed); work.innerHTML += msg[i] } };
const main = async () => {
    speed = 30;
    await sleep(1000);
    await dprint("Hello World\n");
}
main();
```

We can then allow a line to begin with
`]` by prepending it with `\` this will
be used for any special character (even
`\`)

for example:

```lig
\] <- is printed
] <- throws an error
\\ <- is printed
\ <- just prints the space
```

We should then allow for italics and bold
we shall do this with enclosing the italics
text with `*` and `**` respectively.
We will achieve this by adding new print
functions for bold and italics, this
will also continue for all new "effects"

```javascript
const edprint = async (msg, type) => { for(let i=0; i<msg.length; i++) {await sleep(speed); work.innerHTML += "<" + type + ">" msg[i] + "</" + type + ">" } };
```

for example italics would be `edprint(msg, "i");`