#include <cmath>
#include<SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include "MainMenu.h"

using namespace sf;

const int num = 8; //checkpoints
// TODO: use checkpoint to make sure we are on the track.
// Slow speed when not on the track.
int points[num][2] = { 300, 610,
  1270,430,
  1380,2380,
  1900,2460,
  1970,1700,
  2550,1680,
  2560,3150,
  500, 3300 };



struct Car
{
	float speed, angle; int n;
	Vector2f pos;
	Car() { speed = 2; angle = 0; n = 0; }
	void move()
	{
		pos.x += sin(angle) * speed;
		pos.y -= cos(angle) * speed;
	}
	void findTarget()
	{
		float tx = points[n][0];
		float ty = points[n][1];
		float beta = angle - atan2(tx - pos.x, -ty + pos.y);
		if (sin(beta) < 0) angle += 0.005 * speed; else angle -= 0.005 * speed;
		// Check if passed a checkpoint
		if ((pos.x - tx) * (pos.x - tx) + (pos.y - ty) * (pos.y - ty) < 25 * 25) n = (n + 1) % num; // TODO: simplify
	}
};

int main()
{
	sf::IpAddress ip = sf::IpAddress::getLocalAddress();
	sf::TcpSocket socket;
	char connectionType;
	char buffer[2000];
	std::size_t received;
	std::string text = "connected to: ";

	socket.setBlocking(false);


	//Main Menu to control if player wants use the "PLAY" ,"MULTIPLAYER" or "EXIT"
	RenderWindow app_M(VideoMode(700, 600), "Main Menu");
	MainMenu raceMenu(app_M.getSize().x, app_M.getSize().y);

	RenderWindow app(VideoMode(1040, 880), "Car Racing Game!");

	sf::Vector2f lastPos, p2Pos; float p2Angle;

	bool update = false;

		while (app_M.isOpen())
		{
			
			Event m;
			while (app_M.pollEvent(m)) {

				switch (m.type)
				{
				case sf::Event::KeyReleased:
					switch (m.key.code)
					{
					case sf::Keyboard::Up:
						raceMenu.moveUp();
						break;

					case sf::Keyboard::Down:
						raceMenu.moveDown();
						break;


					case sf::Keyboard::Return:
						switch (raceMenu.getPressedOption())
						{
						case 0:
							std::cout << "PLAY has been pressed" << std::endl;
							app_M.close();
							app.isOpen();
							


								break;
						case 1:
							std::cout << "MULTIPLAYER has been pressed" << std::endl;

							std::cout << "Enter (s) for server or (c) for client" << std::endl;
							std::cin >> connectionType;

							if (connectionType == 's')
							{
								sf::TcpListener listener;
								listener.listen(2000);
								listener.accept(socket);
								text += "Server";

							}
							else if (connectionType == 'c')
							{
								socket.connect(ip, 2000);
								text += "Client";
							}

							socket.send(text.c_str(), text.length() + 1);

							socket.receive(buffer, sizeof(buffer), received);

							std::cout << buffer << std::endl;

							app_M.close();
							


							break;
						case 2:
							app_M.close();
							app.close();
							break;
						}
					}

					break;

				case sf::Event::Closed:

					app_M.close();
					

					break;
				}
					
			}

			app_M.clear(sf::Color::Black);

			raceMenu.draw(app_M);

			app_M.display();


		}

	
	app.setFramerateLimit(60);
	Texture t1, t2;
	t1.loadFromFile("images/background.png");
	t2.loadFromFile("images/car.png");
	t1.setSmooth(true);
	t2.setSmooth(true);
	Sprite sBackground(t1), sCar(t2);
	sBackground.scale(2, 2);
	sCar.setOrigin(22, 22);
	float R = 22;
	const int N = 2;
	Car car[N];

	// Starting positions
	for (int i = 0; i < N; i++)
	{
		car[i].pos.x = 300 + i * 50;
		car[i].pos.y = 1700 + i * 80;
		car[i].speed = 7 + i;
	}
	float speed = 0, angle = 0;
	float maxSpeed = 12.0;
	float acc = 0.2, dec = 0.3;
	float turnSpeed = 0.08;
	int offsetX = 0, offsetY = 0;

	sf::View track(sf::FloatRect(0.f, 0.f, 2800.0f, 3600.0f));

	while (app.isOpen())
	{
		Event e;
		while (app.pollEvent(e))
		{
			if (e.type == Event::Closed)
				app.close();
			else if (e.type == sf::Event::GainedFocus)
				update = true;
			else if (e.type == sf::Event::LostFocus)
				update = false;
		}
		bool Up = 0, Right = 0, Down = 0, Left = 0;


		

		lastPos = car[0].pos;
		if (update) {
			if (Keyboard::isKeyPressed(Keyboard::Up)) Up = 1;
			if (Keyboard::isKeyPressed(Keyboard::Right)) Right = 1;
			if (Keyboard::isKeyPressed(Keyboard::Down)) Down = 1;
			if (Keyboard::isKeyPressed(Keyboard::Left)) Left = 1;
		}
		sf::Packet packet;

		if (lastPos != car[0].pos);
		{
			packet << car[0].pos.x << car[0].pos.y << car[0].angle;
			socket.send(packet);
		}

		socket.receive(packet);
		if (packet >> p2Pos.x >> p2Pos.y >> p2Angle)
		{
			car[1].pos = p2Pos;
			car[1].angle = p2Angle;
		}

		//car movement
		if (Up && speed < maxSpeed)
		{
			if (speed < 0)  speed += dec;
			else  speed += acc;
		}
		if (Down && speed > -maxSpeed)
		{
			if (speed > 0) speed -= dec;
			else  speed -= acc;
		}
		if (!Up && !Down)
		{
			if (speed - dec > 0) speed -= dec;
			else if (speed + dec < 0) speed += dec;
			else speed = 0;
		}
		if (Right && speed != 0)  angle += turnSpeed * speed / maxSpeed;
		if (Left && speed != 0)   angle -= turnSpeed * speed / maxSpeed;

		car[0].speed = speed;
		car[0].angle = angle;
		for (int i = 0; i < N; i++) car[i].move();
		for (int i = 1; i < N; i++) car[i].findTarget();
		//collision

		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
			{
				if (i == j) {
					break;
				}
				int dx = 0, dy = 0;
				while (dx * dx + dy * dy < 4 * R * R)
				{
					car[i].pos.x += dx / 10.0;
					car[i].pos.x += dy / 10.0;
					car[j].pos.x -= dx / 10.0;
					car[j].pos.y -= dy / 10.0;
					dx = car[i].pos.x - car[j].pos.x;
					dy = car[i].pos.y - car[j].pos.y;
					if (!dx && !dy) break;
				}
			}
		app.clear(Color::White);


		

		// to make sure cars stay with in the limit of the map.
		if (car[0].pos.x > 2860 - 90) car[0].pos.x = 2860 - 90;

		if (car[0].pos.y > 3655 - 90) car[0].pos.y = 3655 - 90;

		if (car[0].pos.x < 0 + 22) car[0].pos.x = 0 + 22 ;

		if (car[0].pos.y < 0 + 22) car[0].pos.y = 0 + 22;


		/*if (car[0].pos.x > 320) offsetX = car[0].pos.x - 320;
		if (car[0].pos.y > 240) offsetY = car[0].pos.y - 240;*/
		/*sBackground.setPosition(-offsetX, -offsetY);*/
		
		// creating the checkpoints
		sf::RectangleShape rc(sf::Vector2f(160, 50));
		sf::RectangleShape rc1(sf::Vector2f(160, 50));
		sf::RectangleShape rc2(sf::Vector2f(160, 50));
		sf::RectangleShape rc3(sf::Vector2f(160, 50));
		sf::RectangleShape rc4(sf::Vector2f(160, 50));
		sf::RectangleShape rc5(sf::Vector2f(160, 50));
		sf::RectangleShape rc6(sf::Vector2f(40, 200));
		sf::RectangleShape rc7(sf::Vector2f(40, 200));

		
		//this sets the postion of the visual checkpoints
		rc.setPosition(240, 610);
		rc1.setPosition(1300, 610);
		rc2.setPosition(1300, 2380);
		rc3.setPosition(1840, 2360);
		rc4.setPosition(1840, 1700);
		rc5.setPosition(2490, 1880);
		rc6.setPosition(2200, 3200);
		rc7.setPosition(500, 3200);


		//checkpoints colour outline color and thickness
		rc.setFillColor(sf::Color::White);
		rc.setOutlineColor(sf::Color::Green);
		rc.setOutlineThickness(2);

		rc1.setFillColor(sf::Color::White);
		rc1.setOutlineColor(sf::Color::Green);
		rc1.setOutlineThickness(2);

		rc2.setFillColor(sf::Color::White);
		rc2.setOutlineColor(sf::Color::Green);
		rc2.setOutlineThickness(2);

		rc3.setFillColor(sf::Color::White);
		rc3.setOutlineColor(sf::Color::Green);
		rc3.setOutlineThickness(2);

		rc4.setFillColor(sf::Color::White);
		rc4.setOutlineColor(sf::Color::Green);
		rc4.setOutlineThickness(2);

		rc5.setFillColor(sf::Color::White);
		rc5.setOutlineColor(sf::Color::Green);
		rc5.setOutlineThickness(2);

		rc6.setFillColor(sf::Color::White);
		rc6.setOutlineColor(sf::Color::Green);
		rc6.setOutlineThickness(2);

		rc7.setFillColor(sf::Color::White);
		rc7.setOutlineColor(sf::Color::Green);
		rc7.setOutlineThickness(2);

		
		//draws the view of the track so the white is no longer seen.
		app.setView(track);
		
		
		app.draw(sBackground);
		


		//draws checkpoints
		app.draw(rc);
		app.draw(rc1);
		app.draw(rc2);
		app.draw(rc3);
		app.draw(rc4);
		app.draw(rc5);
		app.draw(rc6);
		app.draw(rc7);

		


		Color colors[10] = { Color::Red, Color::Green, Color::Magenta, Color::Blue, Color::White };
		for (int i = 0; i < N; i++)
		{
			sCar.setPosition(car[i].pos.x - offsetX, car[i].pos.y - offsetY);
			sCar.setRotation(car[i].angle * 180 / 3.141593);
			sCar.setColor(colors[i]);
			app.draw(sCar);
			
		}

		app.display();
	}
	return 0;
}