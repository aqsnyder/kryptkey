/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body with text wrapping
  ******************************************************************************
  * @attention
  *
  * This code modifies the show_account_details() function to wrap long usernames
  * and passwords to the next line if they are too long for the display.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"        // Include your SSD1306 driver
#include "ssd1306_fonts.h"  // Include the fonts you need
#include "aes.h"            // Include TinyAES library
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// No user typedefs
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEBOUNCE_DELAY 20             // 20ms debounce time
#define ENCRYPTED_PASSWORD_LENGTH 16  // All passwords are 16 bytes
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
// No user macros
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */
// Account data
const char* account_names[] = {
    "amazon",
    "bank_amex",
    "bank_chase",
    "bank_discover",
    "bank_pnc",
    "card_amazon",
    "card_discover",
    "card_gold",
    "card_pnc_credit",
    "card_pnc_debit",
    "coinbase",
    "digikey",
    "discord",
    "github",
    "gmail",
    "go_daddy",
    "invictus_3dexp",
    "invictus_outlook",
    "invictus_quikbooks",
    "mouser",
    "nationwide_energy",
    "nord",
    "spotify",
    "state_farm",
    "turbo_tax",
    "tu_portal",
    "twitter",
    "ups",
    "wix"
};

const char* usernames[] = {
    "tuf50288@temple.edu",
    "bank_american",
    "bank_chase",
    "snyderdiscover9",
    "snyderonlinepnc",
    "null",
    "null",
    "null",
    "null",
    "null",
    "tuf50288@temple.edu",
    "aaron@isiwireless.com",
    "aaron.q.snyder@gmail.com",
    "aaron.q.snyder@gmail.com",
    "aaron.q.snyder@gmail.com",
    "aaron@isiwireless.com",
    "aaron@isiwireless.com",
    "aaron@isiwireless.com",
    "aaron@isiwireless.com",
    "aaron@isiwireless.com",
    "aaron.q.snyder@gmail.com",
    "aaron.snyder@temple.edu",
    "tuf50288@temple.edu",
    "SnyderFarm9",
    "aaron.q.snyder@gmail.com",
    "tuf50288@temple.edu",
    "aaron3151489061",
    "snyder_ups",
    "aaron.snyder@temple.edu"
};

int current_selection = 0;  // Tracks the currently selected item

// Encrypted passwords
uint8_t encrypted_passwords[][16] = {
    {0x18, 0x42, 0x40, 0x49, 0x2c, 0x71, 0xf5, 0x02, 0xf2, 0x4b, 0x17, 0x90, 0x79, 0xfa, 0xd6, 0x0b},
    {0xf1, 0xc6, 0x0a, 0x1d, 0x56, 0x61, 0xdb, 0xf1, 0x7f, 0xc8, 0xa6, 0xb0, 0x3f, 0xdd, 0x8d, 0x76},
    {0x78, 0x91, 0xad, 0xa7, 0x2a, 0x43, 0x71, 0xa6, 0x9b, 0x71, 0xc9, 0x60, 0x1c, 0x0b, 0xe1, 0xa3},
    {0x34, 0xe9, 0xa3, 0x5a, 0xa2, 0xa1, 0x47, 0x34, 0x6d, 0x15, 0xee, 0x7e, 0xb5, 0xf5, 0x5d, 0xa6},
    {0x03, 0x72, 0xeb, 0x44, 0x3d, 0x56, 0x44, 0xd2, 0x10, 0xff, 0xaa, 0xaf, 0xe8, 0x02, 0x74, 0xf0},
    {0x0d, 0x0e, 0x6a, 0x20, 0x95, 0x6f, 0x23, 0x96, 0x08, 0xcb, 0x9e, 0x09, 0xe5, 0xb2, 0xe0, 0x94},
    {0x1d, 0x98, 0xf2, 0x0c, 0x1e, 0x48, 0xb2, 0x51, 0xc9, 0x1f, 0xa3, 0x2c, 0xbb, 0x25, 0x40, 0x22},
    {0x5d, 0xff, 0x06, 0xad, 0x67, 0xef, 0x4a, 0x4e, 0xc7, 0xdf, 0x27, 0x84, 0xfa, 0x8e, 0x32, 0x82}
};

// Define screen states
typedef enum {
    STATE_LOGIN,     // Login screen
    STATE_MENU,      // Showing the list of accounts
    STATE_DETAILS    // Showing the username and password
} ScreenState;

ScreenState current_state = STATE_LOGIN;  // Start in the login state

// PIN variables
int pin_input[3] = {0, 0, 0};
int pin_index = 0;  // Index of current digit being set
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
void display_menu(void);
void navigate_menu(int direction);
void check_buttons(void);
void handle_enter_button(void);
void show_account_details(int index);
uint8_t debounce_button(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void display_login_screen(void);
void handle_login_buttons(void);
void derive_key_from_pin(int pin_digits[], uint8_t key[16]);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// No user code before main
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
  /* USER CODE BEGIN 1 */
  // No user code here
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  // No user code here
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  // No user code here
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();

  /* USER CODE BEGIN 2 */
  ssd1306_Init();  // Initialize the OLED display
  display_login_screen();  // Show the login screen
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    if (current_state == STATE_LOGIN) {
        handle_login_buttons();  // Handle login input
    } else {
        check_buttons();  // Poll buttons for menu navigation
    }
    HAL_Delay(5);   // Add delay to avoid button bouncing
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void) {
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure GPIO pins : PA4 PA5 PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;  // Enable pull-down resistors to prevent floating pins
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
  __disable_irq();
  while (1) {
  }
}

/* USER CODE BEGIN 4 */

/* Display the menu of account names with scrolling */
void display_menu() {
    ssd1306_Fill(Black);  // Clear the screen

    int menu_size = sizeof(account_names) / sizeof(account_names[0]);
    int window_size = 4;  // Number of items that can fit on the screen
    int start_index;

    if (menu_size <= window_size) {
        // All items fit on the screen
        start_index = 0;
        window_size = menu_size;  // Adjust window_size if fewer items
    } else {
        // Ensure the current_selection is centered in the window when possible
        start_index = current_selection - window_size / 2;

        // Adjust start_index to stay within valid bounds
        if (start_index < 0) {
            start_index = 0;
        } else if (start_index > menu_size - window_size) {
            start_index = menu_size - window_size;
        }
    }

    // Display the menu items within the window
    for (int i = 0; i < window_size; i++) {
        int item_index = start_index + i;

        ssd1306_SetCursor(2, i * 16);  // Set Y position for each line

        if (item_index == current_selection) {
            ssd1306_WriteString(account_names[item_index], Font_11x18, White);  // Highlight selected item
        } else {
            ssd1306_WriteString(account_names[item_index], Font_7x10, White);
        }
    }

    ssd1306_UpdateScreen();  // Send buffer to display
}

/* Handle menu navigation based on button input */
void navigate_menu(int direction) {
    int menu_size = sizeof(account_names) / sizeof(account_names[0]);
    current_selection += direction;
    if (current_selection < 0) {
        current_selection = menu_size - 1;  // Wrap to bottom
    } else if (current_selection >= menu_size) {
        current_selection = 0;  // Wrap to top
    }
    display_menu();  // Update the display
}

/* Toggle between menu and account details */
void handle_enter_button() {
    if (current_state == STATE_MENU) {
        // Switch to showing account details
        show_account_details(current_selection);
        current_state = STATE_DETAILS;
    } else if (current_state == STATE_DETAILS) {
        // Switch back to the menu
        display_menu();
        current_state = STATE_MENU;
    }
}

/* Display account details when selected */
void show_account_details(int index) {
    uint8_t key[16];
    derive_key_from_pin(pin_input, key);

    size_t encrypted_length = ENCRYPTED_PASSWORD_LENGTH;
    uint8_t decrypted_password[64];
    memcpy(decrypted_password, encrypted_passwords[index], encrypted_length);

    struct AES_ctx ctx;
    AES_init_ctx(&ctx, key);

    // Decrypt the password in ECB mode
    for (size_t i = 0; i < encrypted_length; i += 16) {
        AES_ECB_decrypt(&ctx, decrypted_password + i);
    }

    // Remove padding (PKCS#7)
    size_t pad_len = decrypted_password[encrypted_length - 1];
    size_t decrypted_length = encrypted_length - pad_len;

    // Ensure the decrypted password is null-terminated
    decrypted_password[decrypted_length] = '\0';

    // Display the decrypted password
    ssd1306_Fill(Black);

    int max_chars_per_line = 21;  // For Font_6x8
    int y = 0;  // Starting Y position

    // Display "Username:"
    ssd1306_SetCursor(2, y);
    ssd1306_WriteString("Username:", Font_6x8, White);
    y += 8;  // Move to next line

    // Display the username with wrapping
    const char* username = usernames[index];
    int username_len = strlen(username);
    int start = 0;
    while (start < username_len && y < 64) {  // Ensure we don't exceed the display height
        char line[22];  // max_chars_per_line + 1 for null terminator
        int line_length = username_len - start;
        if (line_length > max_chars_per_line) {
            line_length = max_chars_per_line;
        }
        strncpy(line, username + start, line_length);
        line[line_length] = '\0';
        ssd1306_SetCursor(2, y);
        ssd1306_WriteString(line, Font_6x8, White);
        y += 8;  // Move to next line
        start += line_length;
    }

    // Leave a blank line before "Password:"
    y += 4;

    // Display "Password:"
    if (y < 64) {
        ssd1306_SetCursor(2, y);
        ssd1306_WriteString("Password:", Font_6x8, White);
        y += 8;
    }

    // Display the password with wrapping
    char* password = (char*)decrypted_password;
    int password_len = strlen(password);
    start = 0;
    while (start < password_len && y < 64) {
        char line[22];  // max_chars_per_line + 1
        int line_length = password_len - start;
        if (line_length > max_chars_per_line) {
            line_length = max_chars_per_line;
        }
        strncpy(line, password + start, line_length);
        line[line_length] = '\0';
        ssd1306_SetCursor(2, y);
        ssd1306_WriteString(line, Font_6x8, White);
        y += 8;  // Move to next line
        start += line_length;
    }

    ssd1306_UpdateScreen();
}

/* Check the state of buttons and navigate the menu */
void check_buttons() {
    if (debounce_button(GPIOA, GPIO_PIN_6)) {  // First button
        if (current_state == STATE_MENU) {
            navigate_menu(-1);  // Move up in the list
        }
    } else if (debounce_button(GPIOA, GPIO_PIN_5)) {  // Second button
        if (current_state == STATE_MENU) {
            navigate_menu(1);   // Move down in the list
        }
    } else if (debounce_button(GPIOA, GPIO_PIN_4)) {
        handle_enter_button();  // Toggle between the menu and account details
    }
}

/* Debounce button presses */
uint8_t debounce_button(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET) {
        HAL_Delay(DEBOUNCE_DELAY);  // Wait for debounce time
        if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET) {
            while (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET);  // Wait until button is released
            return 1;  // Button is confirmed pressed
        }
    }
    return 0;  // Button is not pressed
}

/* Display the login screen */
void display_login_screen() {
    ssd1306_Fill(Black);  // Clear the screen

    ssd1306_SetCursor(2, 0);
    ssd1306_WriteString("Enter PIN:", Font_11x18, White);

    // Display the PIN digits
    char pin_display[10];
    sprintf(pin_display, "%d %d %d", pin_input[0], pin_input[1], pin_input[2]);

    ssd1306_SetCursor(2, 24);
    ssd1306_WriteString(pin_display, Font_11x18, White);

    // Underline the current digit
    ssd1306_SetCursor(2 + pin_index * 24, 44);  // Adjust position based on digit index
    ssd1306_WriteString("^", Font_11x18, White);

    ssd1306_UpdateScreen();  // Send buffer to display
}

/* Handle button inputs on the login screen */
void handle_login_buttons() {
    if (debounce_button(GPIOA, GPIO_PIN_6)) {
        // Increase current digit
        pin_input[pin_index] = (pin_input[pin_index] + 1) % 10;  // Digits 0-9
        display_login_screen();
    } else if (debounce_button(GPIOA, GPIO_PIN_5)) {
        // Move to next digit
        pin_index = (pin_index + 1) % 3;  // Wrap around 0-2
        display_login_screen();
    } else if (debounce_button(GPIOA, GPIO_PIN_4)) {
        // Confirm PIN
        uint8_t key[16];
        derive_key_from_pin(pin_input, key);

        // Attempt to decrypt the first password
        size_t encrypted_length = ENCRYPTED_PASSWORD_LENGTH;
        uint8_t decrypted_password[64];  // Adjust size as needed
        memcpy(decrypted_password, encrypted_passwords[0], encrypted_length);

        struct AES_ctx ctx;
        AES_init_ctx(&ctx, key);

        for (size_t i = 0; i < encrypted_length; i += 16) {
            AES_ECB_decrypt(&ctx, decrypted_password + i);
        }

        // Remove padding
        size_t pad_len = decrypted_password[encrypted_length - 1];
        size_t decrypted_length = encrypted_length - pad_len;
        decrypted_password[decrypted_length] = '\0';

        // Check if the decrypted password matches "password1"
        if (strcmp((char *)decrypted_password, "password1") == 0) {
            // Correct PIN
            current_state = STATE_MENU;
            display_menu();
        } else {
            // Incorrect PIN, reset input
            pin_input[0] = 0;
            pin_input[1] = 0;
            pin_input[2] = 0;
            pin_index = 0;
            display_login_screen();
        }
    }
}

/* Derive AES key from 3-digit PIN */
void derive_key_from_pin(int pin_digits[], uint8_t key[16]) {
    for (int i = 0; i < 16; i++) {
        key[i] = (uint8_t)(pin_digits[i % 3]);
    }
}

/* USER CODE END 4 */
