#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>
#include <sstream>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#define PI 3.14159



using namespace std;

sf::Sprite bat1, bat2, line, ball;
sf::Texture bat;
sf::Texture centreLine;
sf::Texture squareBall;
sf::Text scoreLeft, scoreRight, gameMode, difficultyLevel, achievementHeader, achievementInfo;
sf::Font font;

int pointsLeft = 0;
int pointsRight = 0;
int randPos = (rand()%32);

float PADDLE_SPEED = 500;
float BALL_SPEED = 750;

void playGame(unsigned short);
float originX(sf::Sprite);
float originY(sf::Sprite);

sf::UdpSocket socketClient;
sf::UdpSocket socketServer;

sf::RenderWindow window(sf::VideoMode(800, 600), "Harry's Bat and Ball Extravaganza", sf::Style::Close);
sf::RenderWindow achievement(sf::VideoMode(30, 120), " ", sf::Style::None);


int main()
{

    int difficulty = 0;

    bat.loadFromFile("resources/Thing.png");
    centreLine.loadFromFile("resources/Line.png");
    squareBall.loadFromFile("resources/Ball.png");
    font.loadFromFile("resources/Lato-Lig.otf");

    bat1.setTexture(bat);
    bat2.setTexture(bat);
    line.setTexture(centreLine);
    ball.setTexture(squareBall);

    bat1.setPosition(5, 268);
    bat2.setPosition(775, 268);
    line.setPosition(396,0);
    ball.setPosition(800 / 2, 600 / 2);

    int mainScreen();
    mainScreen();

//    cout << "Welcome to Harry's Bat and Ball Extravaganza!" << endl;
//    cout << "Please select a game mode. Press S for single-player, or any other button for multiplayer." << endl;
//    string mode;
//    std::cin >> mode;
//
//    if (mode == "S" || mode == "s")
//    {
//        while (difficulty < 1 || difficulty > 4)
//        {
//            cout << "Press 1 for Easy mode, 2 for Medium and 3 for Hard. Press 4 for a practise! ";
//            std::cin >> difficulty;
//        }
//        playGame(difficulty);
//    }
//    else
//        playGame(5);
//    return 0;
}

void playGame(unsigned short difficulty)
{

    sf::Clock timeKeep;
    float lastTime;
    float curTime;
    float delta;
    bool inside = false;

    float dir = 0;
    float difficultyMovement = 0;
    string difficultyLevelText;

    switch(difficulty)
    {
        case 1:
            difficultyLevelText = "EASY";
            difficultyMovement = 0.5;
            break;
        case 2:
            difficultyLevelText = "MEDIUM";
            difficultyMovement = 0.62;
            break;
        case 3:
            difficultyLevelText = "HARD";
            difficultyMovement = 0.72;
            break;
        case 4:
            difficultyMovement = 1;
            break;
        default:
            break;
    }

    if (difficulty != 4)
    {
        scoreLeft.setFont(font);
        scoreLeft.setString("0");
        scoreLeft.setCharacterSize(200);
        scoreLeft.setColor(sf::Color(200, 200, 200, 225));
        scoreLeft.setPosition(200, 180);

        scoreRight.setFont(font);
        scoreRight.setString("0");
        scoreRight.setCharacterSize(200);
        scoreRight.setColor(sf::Color(200, 200, 200, 225));
        scoreRight.setPosition(490, 180);

        if(difficulty != 5) {
            difficultyLevel.setFont(font);
            difficultyLevel.setString(difficultyLevelText);
            difficultyLevel.setCharacterSize(24);
            difficultyLevel.setColor(sf::Color(200, 200, 200, 225));

            sf::FloatRect textRect = difficultyLevel.getLocalBounds();
            difficultyLevel.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
            difficultyLevel.setPosition(sf::Vector2f(800/2.0f,460));
        }
    }

    else
    {
        scoreLeft.setFont(font);
        scoreLeft.setString("PRACTISE");
        scoreLeft.setCharacterSize(150);
        scoreLeft.setColor(sf::Color(200, 200, 200, 225));

        sf::FloatRect textRect = scoreLeft.getLocalBounds();
        scoreLeft.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
        scoreLeft.setPosition(sf::Vector2f(800/2.0f,180));
    }

    gameMode.setFont(font);
    if(difficulty != 5)
        gameMode.setString("SINGLE   PLAYER");
    else
        gameMode.setString("MULTI   PLAYER");
    gameMode.setCharacterSize(30);
    gameMode.setColor(sf::Color(200, 200, 200, 225));

    sf::FloatRect textRect = gameMode.getLocalBounds();
    gameMode.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
    gameMode.setPosition(sf::Vector2f(800/2.0f,420));

    lastTime = timeKeep.getElapsedTime().asSeconds();

    while (window.isOpen())
    {
        curTime = timeKeep.getElapsedTime().asSeconds();
        delta = curTime - lastTime;
        lastTime = curTime;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            if (bat1.getPosition().y > 1)
                bat1.setPosition(bat1.getPosition().x, bat1.getPosition().y - PADDLE_SPEED*delta);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            if (bat1.getPosition().y < 535)
                bat1.setPosition(bat1.getPosition().x, bat1.getPosition().y + PADDLE_SPEED*delta);
        }
        if(difficulty == 5)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                if (bat2.getPosition().y > 1)
                    bat2.setPosition(bat2.getPosition().x, bat2.getPosition().y - PADDLE_SPEED*delta);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                if (bat2.getPosition().y < 535)
                    bat2.setPosition(bat2.getPosition().x, bat2.getPosition().y + PADDLE_SPEED*delta);
            }
        }

        bool b1 = ball.getGlobalBounds().intersects(bat1.getGlobalBounds());
        bool b2 = ball.getGlobalBounds().intersects(bat2.getGlobalBounds());

        if (b1 || b2)
        {
            int pos = (ball.getPosition().y - (b1 ? bat1.getPosition().y : bat2.getPosition().y));
            if (pos <= 0)
            {
                pos = 1;
                dir = ((pos * 2.109375) / 57.2957795) + ((b1 ? -60 : 150) / 57.2957795);
            }
            else
                dir = ((pos * 2.109375) / 57.2957795) + ((b1 ? -60 : 150) / 57.2957795);
            if(b2)
                dir = -dir;
            if(difficulty != 5 && b2)
                randPos = (rand()%32);
        }

        if ((ball.getPosition().y < 1 || ball.getPosition().y > 584) && !inside) {
            dir = -dir;
            inside = true;
        }
        if((ball.getPosition().y >= 1 && ball.getPosition().y <= 584) && inside)
            inside = false;

        if (ball.getPosition().x < -200 || ball.getPosition().x > 1000)
        {
            if(ball.getPosition().x < -200) {
                dir = 0;
                pointsRight = pointsRight + 1;
                scoreRight.setString(to_string(pointsRight));
            }
            else {
                dir = PI;
                pointsLeft = pointsLeft + 1;
                scoreLeft.setString(to_string(pointsLeft));
            }
            inside = false;
            ball.setPosition(800 / 2, 600 / 2);
            bat1.setPosition(5, 268);
            bat2.setPosition(775, 268);
        }

        if (difficulty != 4)
        {
            if (pointsLeft == 10)
            {
                std::stringstream strm;
                std::string num;
                strm << pointsLeft;
                strm >> num;

                std::stringstream strm1;
                std::string num1;
                strm << pointsRight;
                strm >> num1;

                window.close();
                cout << "Left side wins, with a score of " << num << "-" << num1 << endl;
                system("PAUSE");
            }

            if (pointsRight == 10)
            {
                std::stringstream strm;
                std::string num;
                strm << pointsLeft;
                strm >> num;

                std::stringstream strm1;
                std::string num1;
                strm << pointsRight;
                strm >> num1;

                window.close();
                cout << "Right side wins, with a score of " << num << "-" << num1 << endl;
                system("PAUSE");
            }
        }

        ball.setPosition(ball.getPosition().x + BALL_SPEED*delta * (cos(dir)), ball.getPosition().y + BALL_SPEED*delta * sin(dir));

        if(difficulty != 5) {
            if( difficulty != 4 ) {
                if (ball.getPosition().y - bat2.getPosition().y > randPos)
                    bat2.setPosition(bat2.getPosition().x, bat2.getPosition().y + difficultyMovement);
                else if (ball.getPosition().y - bat2.getPosition().y < randPos)
                    bat2.setPosition(bat2.getPosition().x, bat2.getPosition().y - difficultyMovement);

                if (bat2.getPosition().y > 535)
                    bat2.setPosition(bat2.getPosition().x, 535);
                else if (bat2.getPosition().y < 1)
                     bat2.setPosition(bat2.getPosition().x, 1);
            }
            else {
                bat2.setPosition(bat2.getPosition().x, ball.getPosition().y - randPos);
            }
        }

        window.clear(sf::Color::White);
        window.draw(line);
        window.draw(scoreLeft);
        window.draw(scoreRight);
        window.draw(gameMode);
        window.draw(difficultyLevel);
        window.draw(bat1);
        window.draw(bat2);
        window.draw(ball);
        window.display();
    }
}

void achievementGet()
{
    sf::Sprite windowBg;
    sf::Texture windowBgTexture;

    achievementHeader.setFont(font);
    achievementHeader.setString("Achievement Get!");
    achievementHeader.setCharacterSize(30);
    achievementHeader.setColor(sf::Color(255, 255, 255, 255));
    achievementHeader.setPosition(4, 4);
        scoreLeft.setColor(sf::Color(200, 200, 200, 225));
}

int mainScreen()
{

    void playGame(unsigned short);

    if (!font.loadFromFile("resources/Lato-Lig.otf"))
    {
            cout << "Error loading fonts." << endl;
    }
    sf::Sprite spHeader, mpHeader, easyButton, medButton, hardButton, pracButton, mpButton, logo;
    sf::Texture headerSP, headerMP, logoTexture, btnEasyOff, btnEasyOn, btnMedOff, btnMedOn, btnHardOff, btnHardOn, btnPracOff, btnPracOn, btnMultiOff, btnMultiOn;

    headerSP.loadFromFile("resources/HEADER_singleplayer.png");
    headerMP.loadFromFile("resources/HEADER_multiplayer.png");
    logoTexture.loadFromFile("resources/LOGO.png");
    btnEasyOff.loadFromFile("resources/EASY_off.png");
    btnEasyOn.loadFromFile("resources/EASY_on.png");
    btnMedOff.loadFromFile("resources/MEDIUM_off.png");
    btnMedOn.loadFromFile("resources/MEDIUM_on.png");
    btnHardOff.loadFromFile("resources/HARD_off.png");
    btnHardOn.loadFromFile("resources/HARD_on.png");
    btnPracOff.loadFromFile("resources/PRACTISE_off.png");
    btnPracOn.loadFromFile("resources/PRACTISE_on.png");
    btnMultiOff.loadFromFile("resources/MULTIPLAYER_off.png");
    btnMultiOn.loadFromFile("resources/MULTIPLAYER_on.png");

    spHeader.setTexture(headerSP);
    mpHeader.setTexture(headerMP);
    easyButton.setTexture(btnEasyOff);
    medButton.setTexture(btnMedOff);
    hardButton.setTexture(btnHardOff);
    pracButton.setTexture(btnPracOff);
    mpButton.setTexture(btnMultiOff);
    logo.setTexture(logoTexture);

    logo.setOrigin(originX(logo), originY(logo));
    logo.setPosition(sf::Vector2f((800/2.0f), 100));
    spHeader.setOrigin(originX(spHeader), originY(spHeader));
    spHeader.setPosition(sf::Vector2f((800/2.0f), 230));
    mpHeader.setOrigin(originX(mpHeader), originY(mpHeader));
    mpHeader.setPosition(sf::Vector2f((800/2.0f), 460));
    easyButton.setOrigin(originX(easyButton), originY(easyButton));
    easyButton.setPosition(sf::Vector2f((800/6.0f), 295));
    medButton.setOrigin(originX(medButton), originY(medButton));
    medButton.setPosition(sf::Vector2f((800/2.0f), 295));
    hardButton.setOrigin(originX(hardButton), originY(hardButton));
    hardButton.setPosition(sf::Vector2f(5*(800/6.0f), 295));
    pracButton.setOrigin(originX(pracButton), originY(pracButton));
    pracButton.setPosition(sf::Vector2f((800/2.0f), 360));
    mpButton.setOrigin(originX(mpButton), originY(mpButton));
    mpButton.setPosition(sf::Vector2f((800/2.0f), 530));


    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

//        sf::IntRect rect((800/6.0f), 295, 250, 60);
//
//        if (rect.contains(sf::Mouse::getPosition(window)))
//            easyButton.setTexture(btnEasyOn);
//        else
//            easyButton.setTexture(btnEasyOff);

//        int mouseX = sf::Mouse::getPosition().x;
//        int mouseY = sf::Mouse::getPosition().y;
//
//        sf::Vector2i windowPosition = window.getPosition();
//
//        if(mouseX > easyButton.getPosition().x + windowPosition.x && mouseX < ( easyButton.getPosition().x + easyButton.getGlobalBounds().width + windowPosition.x)
//            && mouseY > easyButton.getPosition().y + windowPosition.y + 30  && mouseY < ( easyButton.getPosition().y + easyButton.getGlobalBounds().height + windowPosition.y + 30) )
//        {
//            easyButton.setTexture(btnEasyOn);
//        }
//        else
//            easyButton.setTexture(btnEasyOff);

        if (easyButton.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y))
        {
            easyButton.setTexture(btnEasyOn);
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                playGame(1);
            }
        }
        else
            easyButton.setTexture(btnEasyOff);

        if (medButton.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y))
        {
            medButton.setTexture(btnMedOn);
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                playGame(2);
            }
        }
        else
            medButton.setTexture(btnMedOff);

        if (hardButton.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y))
        {
            hardButton.setTexture(btnHardOn);
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                playGame(3);
            }
        }
        else
            hardButton.setTexture(btnHardOff);

        if (pracButton.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y))
        {
            pracButton.setTexture(btnPracOn);
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                playGame(4);
            }
        }
        else
            pracButton.setTexture(btnPracOff);

        if (mpButton.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y))
        {
            mpButton.setTexture(btnMultiOn);
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                playGame(5);
                //void playGameOnline();
                //playGameOnline();
            }
        }
        else
            mpButton.setTexture(btnMultiOff);


        window.clear(sf::Color::White);
        window.draw(logo);
        window.draw(spHeader);
        window.draw(mpHeader);
        window.draw(easyButton);
        window.draw(medButton);
        window.draw(hardButton);
        window.draw(pracButton);
        window.draw(mpButton);


        window.display();
    }
    return 0;
}

void playGameOnline()
{
    sf::Clock timeKeep;
    float lastTime;
    float curTime;
    float delta;
    bool inside = false;

    float dir = 0;
    float difficultyMovement = 0;
    string difficultyLevelText;

    scoreLeft.setFont(font);
    scoreLeft.setString("0");
    scoreLeft.setCharacterSize(200);
    scoreLeft.setColor(sf::Color(200, 200, 200, 225));
    scoreLeft.setPosition(200, 180);

    scoreRight.setFont(font);
    scoreRight.setString("0");
    scoreRight.setCharacterSize(200);
    scoreRight.setColor(sf::Color(200, 200, 200, 225));
    scoreRight.setPosition(490, 180);

    gameMode.setFont(font);
    gameMode.setString("ONLINE");
    gameMode.setCharacterSize(30);
    gameMode.setColor(sf::Color(200, 200, 200, 225));

    sf::FloatRect textRect = gameMode.getLocalBounds();
    gameMode.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
    gameMode.setPosition(sf::Vector2f(800/2.0f,420));

    lastTime = timeKeep.getElapsedTime().asSeconds();

    bat1.setPosition(bat1.getPosition().x, 100);


    // bind the socket to a port
    if (socketClient.bind(60005) != sf::Socket::Done)
    {
        // error...
    }
    if (socketServer.bind(60006) != sf::Socket::Done)
    {
        // error...
    }
    cout << "Your port is: " << socketServer.getLocalPort() << endl;
    cout << "Your local IP address is: " << sf::IpAddress::getLocalAddress() << endl;
    cout << "Your public IP address is: " << "hjhjh" << endl;

    void onlineClient();
    void onlineServer();


//    cout << "Please enter an IP address to connect to." << endl;
//    sf::IpAddress address;
//    cin >> address;
//
//    sf::TcpSocket socket;
//    sf::Socket::Status status = socket.connect("localhost", 8011);
//    if (status != sf::Socket::Done)
//    {
//        // error...
//    }


    while (window.isOpen())
    {
        curTime = timeKeep.getElapsedTime().asSeconds();
        delta = curTime - lastTime;
        lastTime = curTime;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            if (bat1.getPosition().y > 1)
                bat1.setPosition(bat1.getPosition().x, bat1.getPosition().y - PADDLE_SPEED*delta);

            onlineClient();

        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            if (bat1.getPosition().y < 535)
                bat1.setPosition(bat1.getPosition().x, bat1.getPosition().y + PADDLE_SPEED*delta);

            onlineServer();


        }

        bool b1 = ball.getGlobalBounds().intersects(bat1.getGlobalBounds());
        bool b2 = ball.getGlobalBounds().intersects(bat2.getGlobalBounds());

        if (b1 || b2)
        {
            int pos = (ball.getPosition().y - (b1 ? bat1.getPosition().y : bat2.getPosition().y));
            if (pos <= 0)
            {
                pos = 1;
                dir = ((pos * 2.109375) / 57.2957795) + ((b1 ? -60 : 150) / 57.2957795);
            }
            else
                dir = ((pos * 2.109375) / 57.2957795) + ((b1 ? -60 : 150) / 57.2957795);
            if(b2)
                dir = -dir;
        }

        if ((ball.getPosition().y < 1 || ball.getPosition().y > 584) && !inside) {
            dir = -dir;
            inside = true;
        }
        if((ball.getPosition().y >= 1 && ball.getPosition().y <= 584) && inside)
            inside = false;

        if (ball.getPosition().x < -200 || ball.getPosition().x > 1000)
        {
            if(ball.getPosition().x < -200) {
                dir = 0;
                pointsRight = pointsRight + 1;
                scoreRight.setString(to_string(pointsRight));
            }
            else {
                dir = PI;
                pointsLeft = pointsLeft + 1;
                scoreLeft.setString(to_string(pointsLeft));
            }
            inside = false;
            ball.setPosition(800 / 2, 600 / 2);
            bat1.setPosition(5, 268);
            bat2.setPosition(775, 268);
        }
            if (pointsLeft == 10)
            {
                std::stringstream strm;
                std::string num;
                strm << pointsLeft;
                strm >> num;

                std::stringstream strm1;
                std::string num1;
                strm << pointsRight;
                strm >> num1;

                window.close();
                cout << "Left side wins, with a score of " << num << "-" << num1 << endl;
                system("PAUSE");
            }

            if (pointsRight == 10)
            {
                std::stringstream strm;
                std::string num;
                strm << pointsLeft;
                strm >> num;

                std::stringstream strm1;
                std::string num1;
                strm << pointsRight;
                strm >> num1;

                window.close();
                cout << "Right side wins, with a score of " << num << "-" << num1 << endl;
                system("PAUSE");
            }

        window.clear(sf::Color::White);
        window.draw(line);
        window.draw(scoreLeft);
        window.draw(scoreRight);
        window.draw(gameMode);
        window.draw(difficultyLevel);
        window.draw(bat1);
        window.draw(bat2);
        window.draw(ball);
        window.display();
    }
}


float originX(sf::Sprite sprite)
{
    sf::FloatRect imgRect = sprite.getLocalBounds();
    float oriX = (imgRect.left + imgRect.width/2.0f);
    return oriX;
}

float originY(sf::Sprite sprite)
{
    sf::FloatRect imgRect = sprite.getLocalBounds();
    float oriY = (imgRect.top  + imgRect.height/2.0f);
    return oriY;
}

void onlineClient()
{
    sf::Packet toSend;

    toSend << "Test";

   // sf::IpAddress recipient = "192.168.1.64";
   // unsigned short port = 60006;
    if (socketClient.send(toSend, "192.168.1.64", 60006) != sf::Socket::Done)
    {
        // error...
    }


    // RECEIVE A RESPONSE

    sf::IpAddress sender;
    unsigned short port;

    sf::Packet toReceive;
    if (socketClient.receive(toReceive, sender, port) != sf::Socket::Done)
    {
        // error...
    }

    float y;
    toReceive >> y;

    bat2.setPosition(bat2.getPosition().x, y);
}

void onlineServer()
{

    sf::IpAddress sender;
    unsigned short port;

    cout << "hi";

    sf::Packet toReceive;
    if (socketServer.receive(toReceive, sender, port) != sf::Socket::Done)
    {
        // error...
    }

    cout << "no";

    //  SEND THE BAT POSITION

    sf::Packet toSend;

    float yPos = bat1.getPosition().y;
    toSend << yPos;

    if (socketServer.send(toSend, sender, port) != sf::Socket::Done)
    {
        // error...
    }

}
