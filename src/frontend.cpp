#include <string>

#include "frontend.hpp"
#include "screen.hpp"

using namespace Frontend;

void throwFrontendError(std::string message);

#if BUILD_FOR_RASPI
    //arduino port for frontend
    #include <bcm2835.h>
    #include "joystick.hh"

    Joystick joystick;
    float x_val = 0.0;
    float y_val = 0.0;
    bool A_pressed = false;
    bool B_pressed = false;
    bool ESC_pressed = false;

    //code taken from Display_Lib_RPI GitHub: https://github.com/gavinlyonsrepo/Display_Lib_RPI/blob/main/examples/st7735/Hello_world_SWSPI/main.cpp
    ST7735_TFT myTFT(SCREEN_HEIGHT, SCREEN_WIDTH);

    uint8_t SetupSPI(void); // setup + user options for software SPI

    void Frontend::init() {
        if(SetupSPI()!=0)
            throwFrontendError("Unable to initialize SPI screen!");

        myTFT.TFTfillScreen(ST7735_BLACK);
        myTFT.IMClear();
    }

    void Frontend::cleanUp() {
        myTFT.TFTfillScreen(ST7735_BLACK);
        myTFT.TFTPowerDown();
        bcm2835_close();
    }

    void Frontend::drawScreen() {
        uint16_t color;
        for(int x=0; x<SCREEN_WIDTH; x++) {
            for(int y=0; y<SCREEN_HEIGHT; y++) {
                color = myTFT.Color565(((int16_t)screen.screenBuff[y][x][0]), ((int16_t)screen.screenBuff[y][x][1]), ((int16_t)screen.screenBuff[y][x][2]));
                myTFT.IMDrawPixel(y, SCREEN_WIDTH-x-1, color);
            }
        }

        myTFT.IMDisplay();
    }

    void Frontend::updateReadings() {
        //read from joystick events and update values
        JoystickEvent event;
        if (joystick.sample(&event))
        {
            if (event.isButton()) {
                switch(event.number) {
                    case 1:
                        A_pressed = event.value==0 ? false : true;
                        break;

                    case 2:
                        B_pressed = event.value==0 ? false : true;
                        break;

                    case 9:
                        ESC_pressed = event.value==0 ? false : true;
                        break;

                    default:
                        //button not implemented
                        break;
                }
            } 
            else if(event.isAxis()) {
                switch(event.number) {
                    case 0:
                        if(event.value > 0)
                            x_val = 1;
                        else if(event.value < 0)
                            x_val = -1;
                        else
                            x_val = 0;
                        break;
                    case 1:
                        if(event.value > 0)
                            y_val = -1;
                        else if(event.value < 0)
                            y_val = 1;
                        else
                            y_val = 0;
                        break;
                }
            }
        }
    }

    ST7735_TFT* Frontend::get_tft() {
        return &myTFT;
    }

    float Frontend::getHorAxis() {
        return x_val;
    }

    float Frontend::getVertAxis() {
        return y_val;
    }

    bool Frontend::getABtn() {
        return A_pressed;
    }

    bool Frontend::getBBtn() {
        return B_pressed;
    }

    bool Frontend::getExitBtn() {
        return ESC_pressed;
    }

    uint8_t SetupSPI(void)
    {
        if(!bcm2835_init())
        {
            std::cout << "Error 1201 Problem with init bcm2835 library" << std::endl;
            return 2;
        }

        // GPIO
        int8_t RST_TFT  = 25;
	    int8_t DC_TFT   = 24;

        uint8_t OFFSET_COL = 0;  // 2, These offsets can be adjusted for any issues->
        uint8_t OFFSET_ROW = 0; // 3, with manufacture tolerance/defects at edge of display
        uint16_t TFT_WIDTH = SCREEN_HEIGHT;// Screen width in pixels (128)
        uint16_t TFT_HEIGHT = SCREEN_WIDTH; // Screen height in pixels (160)

        uint16_t SWSPI_CommDelay = 0; //uS GPIO SW SPI delay

        // ** USER OPTION 1 GPIO/SPI TYPE SW **
        myTFT.TFTSetupGPIO(RST_TFT, DC_TFT);
        //********************************************
        // ** USER OPTION 2 Screen Setup **
        myTFT.TFTInitScreenSize(OFFSET_COL, OFFSET_ROW , TFT_WIDTH , TFT_HEIGHT);
        // ***********************************
        // ** USER OPTION 3 PCB_TYPE **
        // pass enum to param1 ,4 choices,see README
        if(myTFT.TFTInitPCBType(myTFT.TFT_ST7735R_Red, SWSPI_CommDelay) != 0)
        {
            bcm2835_close();
            return 3;
        }
        //*****************************

        TFT_MILLISEC_DELAY(50);
        return 0;
    }
#else
    //PC port using opencv
    #include <SFML/Graphics/RenderWindow.hpp>
    #include <SFML/Graphics/Image.hpp>
    #include <SFML/Graphics/Color.hpp>
    #include <SFML/Graphics/Texture.hpp>
    #include <SFML/Graphics/Sprite.hpp>
    
    #include <SFML/Window/Keyboard.hpp>

    sf::RenderWindow window;
    sf::Texture texture;
    sf::Sprite sprite(texture);

    void Frontend::init() {
        window.create(sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "Squiggly Project");
    }

    void Frontend::cleanUp() {
        window.close();
    }

    void Frontend::drawScreen() {
        //create uint8_t version of in-memory buffer
        sf::Image img({SCREEN_WIDTH, SCREEN_HEIGHT}, sf::Color::Black);

        for(unsigned int x=0; x<SCREEN_WIDTH; x++) {
            for(unsigned int y=0; y<SCREEN_HEIGHT; y++) {
                img.setPixel({x, y}, sf::Color(screen.screenBuff[y][x][0], screen.screenBuff[y][x][1], screen.screenBuff[y][x][2], 255));
            }
        }

        window.clear();

        bool t = texture.loadFromImage(img);
        if(!t)
            throwFrontendError("Unable to load screen to SFML!");

        sprite.setTexture(texture, true);
        window.draw(sprite);
        window.display();
    }

    float Frontend::getHorAxis() {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            return 1.0;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            return -1.0;
        else
            return 0.0;
    }

    float Frontend::getVertAxis() {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            return 1.0;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            return -1.0;
        else
            return 0.0;
    }

    bool Frontend::getABtn() {
        return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z);
    }

    bool Frontend::getBBtn() {
        return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X);
    }

    bool Frontend::getExitBtn() {
        return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) || !window.isOpen();
    }
#endif

void throwFrontendError(std::string message) {
    throw std::runtime_error("Program frontend failed! : " + message);
}