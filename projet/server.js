const {readFileSync,writeFileSync} = require('fs');

const express = require('express');
const app = express();


app.get('/',(req,res) => {

    res.send(`
    
        <!DOCTYPE html>
        <html>
            <head>
                <title>
                        Projet IOC
                </title>
            </head>

            <body>
                <h1> IOC <h1>
            </body>

        </html>
    `);


});


/*app.post('/LED',(req,res) => {

    res.send(`
    
    `);


});
*/

app.get('/style.css',(req,res) => {
    res.sendfile(_dirname + "/" + "style.css");

});
app.listen(8000, () => console.log('172.20.10.2'));