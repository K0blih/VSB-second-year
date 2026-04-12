var config = {
    type: Phaser.AUTO,
    scale: {
        parent: 'phaser-app',
        mode: Phaser.Scale.FIT,
        autoCenter: Phaser.Scale.CENTER_BOTH,
        width: 512,
        height: 512
    },
    physics: {
        default: 'arcade',
        arcade: {
            gravity: { y: 0 },
            debug: false
        }
    },
    scene: {
        preload: preload,
        create: create,
        update: update
    },
    debug: true
};

var game = new Phaser.Game(config);

var backgroundLayer;
var collisionLayer;
var objectsLayer;
var map;
var hud;
var settingsButton;
var menuObjects = [];
var player;
var bombs;
var robot;
var collectible;
var cursors;
var move_ctl = false;
var left;
var right;
var up;
var down;

var score = 0;
var bestScore = 0;
var itemTimeoutSeconds = 7;
var nextTimeoutAt = 0;
var pausedItemTimeLeft = 0;
var isGameOver = false;
var isSettingsOpen = false;
var lastGameOverReason = '';
var bombSpeedOptions = [
    { label: 'Slow', value: 120 },
    { label: 'Normal', value: 180 },
    { label: 'Fast', value: 260 }
];
var bombSpeedIndex = 1;
var bgMusic;
var pickupSound;
var clickSound;
var gameOverSound;
var musicEnabled = true;
var sfxEnabled = true;

function preload() {
    this.load.spritesheet('robot', 'assets/lego.png',
        { frameWidth: 37, frameHeight: 48 });
    this.load.spritesheet('player', 'assets/warrior_spritesheet_calciumtrice.png',
        { frameWidth: 32, frameHeight: 32 });
    this.load.spritesheet('items', 'assets/FinishedB.png',
        { frameWidth: 32, frameHeight: 32 });
    this.load.image('bomb', 'assets/bomb.png');
    this.load.image('tiles', 'assets/mountain_landscape.png');
    this.load.tilemapTiledJSON('json_map', 'assets/json_map.json');
    this.load.audio('bgm', 'assets/bgm.wav');
    this.load.audio('pickup', 'assets/pickup.wav');
    this.load.audio('click', 'assets/click.wav');
    this.load.audio('gameover', 'assets/gameover.wav');
}

function resize(width, height) {
    if (!game || !game.scale) {
        return;
    }

    game.scale.refresh();
    fitCameraToMap();
    positionHud();
}

function create() {
    map = this.make.tilemap({ key: 'json_map' });
    var tiles = map.addTilesetImage('map_tiles', 'tiles');

    collisionLayer = map.createLayer('collision', tiles, 0, 0);
    backgroundLayer = map.createLayer('background', tiles, 0, 0);
    objectsLayer = map.createLayer('objects', tiles, 0, 0);
    collisionLayer.setVisible(false);
    collisionLayer.setCollisionByExclusion([-1]);

    this.physics.world.setBounds(0, 0, map.widthInPixels, map.heightInPixels);
    this.cameras.main.setBounds(0, 0, map.widthInPixels, map.heightInPixels);

    player = this.physics.add.sprite(96, 96, 'player');
    player.setCollideWorldBounds(true);
    player.setSize(20, 24).setOffset(6, 6);

    robot = this.physics.add.sprite(224, 224, 'robot');
    robot.setCollideWorldBounds(true);
    robot.setBounce(0, 0);

    this.physics.add.collider(player, collisionLayer);
    this.physics.add.collider(robot, collisionLayer);
    this.cameras.main.roundPixels = true;
    fitCameraToMap();

    collectible = this.physics.add.sprite(0, 0, 'items', 0);
    collectible.body.setAllowGravity(false);
    collectible.setImmovable(true);
    collectible.setDepth(1);

    bombs = this.physics.add.group();
    createBombs.call(this, 4);

    this.physics.add.collider(bombs, collisionLayer);
    this.physics.add.collider(bombs, bombs);
    this.physics.add.overlap(player, collectible, collectItem, null, this);
    this.physics.add.overlap(player, bombs, hitBomb, null, this);
    this.physics.add.overlap(player, robot, hitRobot, null, this);

    hud = this.add.text(0, 16, 'Items picked: 0', {
        fontFamily: 'Arial',
        fontSize: '24px',
        fontStyle: 'bold',
        color: '#ffffff',
        stroke: '#000000',
        strokeThickness: 4
    });
    hud.setOrigin(0.5, 0);
    hud.setDepth(1000);

    settingsButton = this.add.text(0, 16, 'Settings', {
        fontFamily: 'Arial',
        fontSize: '18px',
        fontStyle: 'bold',
        color: '#ffffff',
        backgroundColor: '#111111',
        padding: { x: 10, y: 6 }
    });
    settingsButton.setOrigin(1, 0);
    settingsButton.setDepth(1000);
    settingsButton.setInteractive({ useHandCursor: true });
    settingsButton.on('pointerdown', function() {
        unlockAudio.call(this);
        playSfx(clickSound);
        showSettingsMenu.call(this);
    }, this);

    positionHud();

    this.anims.create({
        key: 'walk',
        frames: this.anims.generateFrameNumbers('player', { start: 11, end: 19 }),
        frameRate: 10,
        repeat: -1
    });

    this.anims.create({
        key: 'robot_run',
        frames: this.anims.generateFrameNumbers('robot', { start: 0, end: 15 }),
        frameRate: 12,
        repeat: -1
    });
    robot.anims.play('robot_run', true);

    this.anims.create({
        key: 'idle',
        frames: this.anims.generateFrameNumbers('player', { start: 1, end: 9 }),
        frameRate: 10,
        repeat: -1
    });

    cursors = this.input.keyboard.createCursorKeys();

    this.input.on('pointerdown', function(pointer) {
        startBackgroundMusic.call(this);
        if (isGameOver || isSettingsOpen || pointer.y < 56) {
            return;
        }
        move_ctl = true;
        pointer_move(pointer);
    });
    this.input.on('pointerup', function() {
        move_ctl = false;
        reset_move();
    });
    this.input.on('pointermove', pointer_move);

    window.addEventListener('resize', function() {
        resize(window.innerWidth, window.innerHeight);
    }, false);
    resize(window.innerWidth, window.innerHeight);

    bgMusic = this.sound.add('bgm', { loop: true, volume: 0.28 });
    pickupSound = this.sound.add('pickup', { volume: 0.55 });
    clickSound = this.sound.add('click', { volume: 0.35 });
    gameOverSound = this.sound.add('gameover', { volume: 0.55 });

    spawnCollectible.call(this);
    updateText.call(this);
}

function fitCameraToMap() {
    if (!game || !game.scene || !game.scene.scenes.length || !map) {
        return;
    }

    var camera = game.scene.scenes[0].cameras.main;
    camera.stopFollow();
    camera.setZoom(1);
    camera.centerOn(map.widthInPixels / 2, map.heightInPixels / 2);
}

function positionHud() {
    if (!hud || !map) {
        return;
    }

    hud.setPosition(map.widthInPixels / 2, 16);

    if (settingsButton) {
        settingsButton.setPosition(map.widthInPixels - 12, 12);
    }
}

function createBombs(count) {
    for (var i = 0; i < count; i += 1) {
        var bombPosition = findSpawnPosition();
        var bomb = bombs.create(bombPosition.x, bombPosition.y, 'bomb');

        bomb.setCollideWorldBounds(true);
        bomb.setBounce(1, 1);
        setRandomBombVelocity(bomb);
    }
}

function setRandomBombVelocity(bomb) {
    var angle = Phaser.Math.FloatBetween(0, Math.PI * 2);
    var speed = getBombSpeed();

    bomb.setVelocity(Math.cos(angle) * speed, Math.sin(angle) * speed);
}

function getBombSpeed() {
    return bombSpeedOptions[bombSpeedIndex].value;
}

function updateBombSpeeds() {
    if (!bombs) {
        return;
    }

    bombs.children.iterate(function(bomb) {
        var velocity;
        var angle;
        var speed;

        if (!bomb || !bomb.body) {
            return;
        }

        velocity = bomb.body.velocity;
        angle = Math.atan2(velocity.y, velocity.x);
        speed = getBombSpeed();

        if (velocity.length() === 0) {
            setRandomBombVelocity(bomb);
        } else {
            bomb.setVelocity(Math.cos(angle) * speed, Math.sin(angle) * speed);
        }
    });
}

function pointer_move(pointer) {
    var dx = 0;
    var dy = 0;
    var min_pointer = (player.body.width + player.body.height) / 4;

    if (move_ctl) {
        reset_move();

        dx = (pointer.worldX - player.x);
        dy = (pointer.worldY - player.y);

        if (Math.abs(dx) > min_pointer) {
            left = (dx < 0);
            right = !left;
        } else {
            left = false;
            right = false;
        }

        if (Math.abs(dy) > min_pointer) {
            up = (dy < 0);
            down = !up;
        } else {
            up = false;
            down = false;
        }
    }
}

function reset_move() {
    up = false;
    down = false;
    left = false;
    right = false;
}

function update(time) {
    var move_x = true;
    var robotSpeed = 55;

    if (isGameOver || isSettingsOpen) {
        return;
    }

    if (move_ctl) {
        pointer_move(game.input.activePointer);
    }

    if (cursors.left.isDown || left) {
        player.body.setVelocityX(-150);
        player.angle = 90;
        player.anims.play('walk', true);
    } else if (cursors.right.isDown || right) {
        player.body.setVelocityX(150);
        player.angle = 270;
        player.anims.play('walk', true);
    } else {
        player.body.setVelocityX(0);
        move_x = false;
    }

    if (cursors.up.isDown || up) {
        player.body.setVelocityY(-150);
        player.angle = 180;
        player.anims.play('walk', true);
    } else if (cursors.down.isDown || down) {
        player.body.setVelocityY(150);
        player.angle = 0;
        player.anims.play('walk', true);
    } else {
        player.body.setVelocityY(0);
        if (!move_x) {
            player.anims.play('idle', true);
        }
    }

    if (time >= nextTimeoutAt) {
        showGameOverMenu.call(this, 'Timeout');
        return;
    }

    this.physics.moveToObject(robot, player, robotSpeed);
    robot.angle = Phaser.Math.RadToDeg(
        Phaser.Math.Angle.Between(robot.x, robot.y, player.x, player.y)
    ) - 90;

    updateText.call(this, time);
}

function updateText(time) {
    if (hud) {
        hud.setText('Items picked: ' + score + ' | Best: ' + bestScore);
    }
    positionHud();
}

function collectItem(playerSprite, item) {
    playSfx(pickupSound);
    score += 1;
    if (score > bestScore) {
        bestScore = score;
    }

    spawnCollectible.call(this);
    updateText.call(this);
}

function hitBomb() {
    showGameOverMenu.call(this, 'Bomb');
}

function hitRobot() {
    showGameOverMenu.call(this, 'Robot');
}

function finishRun() {
    if (score > bestScore) {
        bestScore = score;
    }
}

function restartGame() {
    clearMenu();
    isGameOver = false;
    isSettingsOpen = false;
    lastGameOverReason = '';
    move_ctl = false;
    reset_move();

    score = 0;
    player.enableBody(true, 96, 96, true, true);
    player.setVelocity(0, 0);
    player.angle = 0;
    robot.enableBody(true, 224, 224, true, true);
    robot.setVelocity(0, 0);
    bombs.clear(true, true);
    createBombs.call(this, 4);
    this.physics.resume();
    spawnCollectible.call(this);
    updateText.call(this);
}

function spawnCollectible() {
    var position = findSpawnPosition();
    var frame = Phaser.Math.Between(0, collectible.texture.frameTotal - 2);

    collectible.enableBody(true, position.x, position.y, true, true);
    collectible.setFrame(frame);
    nextTimeoutAt = this.time.now + itemTimeoutSeconds * 1000;
}

function showGameOverMenu(reason) {
    var alreadyGameOver = isGameOver;

    if (!alreadyGameOver) {
        finishRun();
        playSfx(gameOverSound);
    }

    lastGameOverReason = reason || lastGameOverReason || 'Danger';
    isGameOver = true;
    isSettingsOpen = false;
    move_ctl = false;
    reset_move();
    this.physics.pause();

    clearMenu();
    addMenuBackdrop.call(this);
    addMenuText.call(this, 256, 126, 'Game Over', 34);
    addMenuText.call(this, 256, 176, 'Hit by: ' + lastGameOverReason, 20);
    addMenuText.call(this, 256, 206, 'Score: ' + score + '   Best: ' + bestScore, 20);
    addMenuButton.call(this, 256, 268, 'Restart', function() {
        playSfx(clickSound);
        restartGame.call(this);
    });
    addMenuButton.call(this, 256, 324, 'Settings', function() {
        playSfx(clickSound);
        showSettingsMenu.call(this);
    });
    updateText.call(this);
}

function showSettingsMenu() {
    if (isSettingsOpen) {
        return;
    }

    isSettingsOpen = true;
    move_ctl = false;
    reset_move();
    pausedItemTimeLeft = Math.max(1000, nextTimeoutAt - this.time.now);
    this.physics.pause();
    clearMenu();
    addMenuBackdrop.call(this);
    addMenuText.call(this, 256, 96, 'Settings', 32);
    renderSettingsMenu.call(this);
}

function renderSettingsMenu() {
    addMenuText.call(this, 256, 150, 'Bomb speed: ' + bombSpeedOptions[bombSpeedIndex].label, 20);
    addMenuButton.call(this, 184, 198, 'Slower', function() {
        playSfx(clickSound);
        bombSpeedIndex = Math.max(0, bombSpeedIndex - 1);
        updateBombSpeeds();
        showSettingsMenuAgain.call(this);
    });
    addMenuButton.call(this, 328, 198, 'Faster', function() {
        playSfx(clickSound);
        bombSpeedIndex = Math.min(bombSpeedOptions.length - 1, bombSpeedIndex + 1);
        updateBombSpeeds();
        showSettingsMenuAgain.call(this);
    });
    addMenuButton.call(this, 256, 258, 'Music: ' + (musicEnabled ? 'On' : 'Off'), function() {
        playSfx(clickSound);
        musicEnabled = !musicEnabled;
        updateMusicState();
        showSettingsMenuAgain.call(this);
    });
    addMenuButton.call(this, 256, 314, 'SFX: ' + (sfxEnabled ? 'On' : 'Off'), function() {
        sfxEnabled = !sfxEnabled;
        playSfx(clickSound);
        showSettingsMenuAgain.call(this);
    });
    addMenuButton.call(this, 184, 386, 'Resume', function() {
        playSfx(clickSound);
        closeSettingsMenu.call(this);
    });
    addMenuButton.call(this, 328, 386, 'Restart', function() {
        playSfx(clickSound);
        restartGame.call(this);
    });
}

function showSettingsMenuAgain() {
    clearMenu();
    addMenuBackdrop.call(this);
    addMenuText.call(this, 256, 96, 'Settings', 32);
    renderSettingsMenu.call(this);
}

function closeSettingsMenu() {
    clearMenu();
    isSettingsOpen = false;

    if (isGameOver) {
        showGameOverMenu.call(this, lastGameOverReason);
        return;
    }

    nextTimeoutAt = this.time.now + pausedItemTimeLeft;
    this.physics.resume();
}

function addMenuBackdrop() {
    var shade = this.add.rectangle(256, 256, 512, 512, 0x000000, 0.72);
    var panel = this.add.rectangle(256, 256, 360, 360, 0x202020, 0.96);
    panel.setStrokeStyle(2, 0xffffff, 0.85);
    shade.setDepth(2000);
    panel.setDepth(2001);
    menuObjects.push(shade, panel);
}

function addMenuText(x, y, text, size) {
    var item = game.scene.scenes[0].add.text(x, y, text, {
        fontFamily: 'Arial',
        fontSize: size + 'px',
        fontStyle: 'bold',
        color: '#ffffff',
        align: 'center',
        stroke: '#000000',
        strokeThickness: 3
    });
    item.setOrigin(0.5);
    item.setDepth(2002);
    menuObjects.push(item);
    return item;
}

function addMenuButton(x, y, label, callback) {
    var button = game.scene.scenes[0].add.text(x, y, label, {
        fontFamily: 'Arial',
        fontSize: '20px',
        fontStyle: 'bold',
        color: '#ffffff',
        backgroundColor: '#305c89',
        padding: { x: 16, y: 10 }
    });
    button.setOrigin(0.5);
    button.setDepth(2002);
    button.setInteractive({ useHandCursor: true });
    button.on('pointerdown', callback, game.scene.scenes[0]);
    menuObjects.push(button);
    return button;
}

function clearMenu() {
    menuObjects.forEach(function(item) {
        if (item) {
            item.destroy();
        }
    });
    menuObjects = [];
}

function playSfx(sound) {
    if (sfxEnabled && sound) {
        sound.play();
    }
}

function startBackgroundMusic() {
    unlockAudio.call(this);

    if (musicEnabled && bgMusic && !bgMusic.isPlaying) {
        bgMusic.play();
    }
}

function unlockAudio() {
    var context = this && this.sound ? this.sound.context : null;

    if (context && context.state === 'suspended') {
        context.resume();
    }
}

function updateMusicState() {
    if (!bgMusic) {
        return;
    }

    if (musicEnabled) {
        startBackgroundMusic.call(game.scene.scenes[0]);
    } else {
        bgMusic.stop();
    }
}

function findSpawnPosition() {
    var tileX;
    var tileY;
    var tile;
    var x;
    var y;
    var valid;

    do {
        valid = true;
        tileX = Phaser.Math.Between(0, map.width - 1);
        tileY = Phaser.Math.Between(0, map.height - 1);
        x = tileX * map.tileWidth + map.tileWidth / 2;
        y = tileY * map.tileHeight + map.tileHeight / 2;
        tile = collisionLayer ? collisionLayer.getTileAt(tileX, tileY) : null;

        if (tile !== null) {
            valid = false;
        }

        if (valid && player && Phaser.Math.Distance.Between(x, y, player.x, player.y) < map.tileWidth * 2) {
            valid = false;
        }

        if (valid && bombs) {
            bombs.children.iterate(function(bomb) {
                if (bomb && Phaser.Math.Distance.Between(x, y, bomb.x, bomb.y) < map.tileWidth * 1.5) {
                    valid = false;
                }
            });
        }
    } while (!valid);

    return {
        x: x,
        y: y
    };
}
