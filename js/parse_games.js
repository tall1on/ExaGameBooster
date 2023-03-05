'use strict';

const axios = require('axios');
const fs = require('fs');

axios.get('https://discord.com/api/v8/applications/detectable').then(({status, data}) => {
    console.log(status);

    if (200 !== status) {
        throw new Error('Fetching games failed');
    }

    fs.writeFileSync('../assets/games_raw.json', JSON.stringify(data));

    const games = [];
    for (let game of data)
    {
        if (game.executables) {
            for (let executable of game.executables)
            {
                if (executable.os !== 'win32')
                {
                    continue;
                }

                let exeName = executable.name.split('/').pop();

                if (games.includes(exeName))
                {
                    continue;
                }

                games.push(exeName);
            }
        }
    }

    for (let game of JSON.parse(fs.readFileSync('../assets/additional_games.json')))
    {
        console.log(game)

        if (games.includes(game))
        {
            continue;
        }

        games.push(game);
    }

    console.log(`Writing support for ${games.length} games`)

    fs.writeFileSync('../assets/games.json', JSON.stringify(games));
});
