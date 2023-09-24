'use strict';

const axios = require('axios');
const fs = require('fs');

axios.get('https://discord.com/api/v10/applications/detectable').then(({status, data}) => {
    console.log(status);

    if (200 !== status) {
        throw new Error('Fetching games failed');
    }

    fs.writeFileSync('../assets/games_raw.json', JSON.stringify(data));

    const games = [];
    const exeToNameMap = {};
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

                exeToNameMap[exeName] = game.aliases;
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

    let gamesList = `# Supported games
\rCurrently supported games: ${games.length}\n\r`;
    for (let game of games)
    {
        if (!exeToNameMap[game])
        {
            continue;
        }

        if (gamesList.includes(exeToNameMap[game][0]))
        {
            continue;
        }

        gamesList += `- ${exeToNameMap[game][0]}\n\r`;
    }

    fs.writeFileSync('../supportedGames.md', gamesList);
    fs.writeFileSync('../assets/games.json', JSON.stringify(games));
});
