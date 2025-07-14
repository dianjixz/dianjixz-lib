#include "flash.h"

__IO uint8_t page60_buffer[16] = {0};

__IO uint16_t page61_buffer[8] = {0};

__IO bool page60_update_flag = false;

__IO bool page61_update_flag = false;

extern __IO uint8_t g_mb_coils[MB_COIL_COUNT + 1];

extern __IO uint16_t g_mb_holding_regs[MB_HOLDING_REG_COUNT + 1];

/**
 * @brief Get the page number based on the address.
 * @note This function calculates the page number by determining how far
 *       the provided address is from the starting address of the first flash page.
 *
 * @param addr The memory address to check.
 * @retval The page number corresponding to the provided address.
 */
static uint32_t get_page(uint32_t addr)
{
    return (addr - STM32G4xx_FLASH_PAGE0_STARTADDR) / STM32G4xx_PAGE_SIZE;  // Return the page number
}

/**
 * @brief Modify a specific byte of an 8-byte value.
 * @note This function clears the byte at the specified index in the 64-bit data
 *       and sets it to the new value. The byte index is multiplied by 8 to
 *       calculate the correct bit position.
 *
 * @param data The pointer to the 64-bit data to be modified.
 * @param byte_index The index of the byte to modify (0-7).
 * @param new_value The new byte value to set (0-255).
 * @retval None
 */
static void set_byte_in_uint64(uint64_t *data, uint8_t byte_index, uint8_t new_value)
{
    // Clear the byte at the specified index
    *data &= ~((uint64_t)(0xFF) << (byte_index * 8));  // Clear the byte
    // Set the new byte value
    *data |= (uint64_t)new_value << (byte_index * 8);  // Update the byte
}

/**
 * @brief Get the value of a specific byte of an 8-byte (64-bit) value.
 * @note This function extracts the byte at the specified index in the 64-bit data.
 *
 * @param data The 64-bit data to read.
 * @param byte_index The index of the byte to read (0-7).
 * @return The value of the byte (0-255).
 */
static uint8_t get_byte_in_uint64(uint64_t data, uint8_t byte_index)
{
    return (uint8_t)((data >> (byte_index * 8)) & 0xFF);
}

/**
 * @brief Set a 16-bit value in a uint64_t by 16-bit index.
 * @param data Pointer to the 64-bit data.
 * @param index 16-bit index (0–3).
 * @param value The 16-bit value to set.
 */
static void set_2byte_in_uint64(uint64_t *data, uint8_t index, uint16_t value)
{
    *data &= ~((uint64_t)0xFFFF << (index * 16));
    *data |= ((uint64_t)value << (index * 16));
}

/**
 * @brief Get a 16-bit value in a uint64_t by 16-bit index.
 * @param data The 64-bit value.
 * @param index 16-bit index (0–3).
 * @return The 16-bit extracted value.
 */
static uint16_t get_2byte_in_uint64(uint64_t data, uint8_t index)
{
    return (uint16_t)((data >> (index * 16)) & 0xFFFF);
}

/**
 * @brief Read a double word (64 bits) from a given memory address.
 * @note This function dereferences the provided address and returns the 64-bit
 *       value stored there. It uses the volatile pointer type to ensure that
 *       the compiler does not optimize out the read operation.
 *
 * @param address The memory address from which to read.
 * @retval The 64-bit value read from the memory address.
 */
static uint64_t my_flash_read_double_word(uint32_t address)
{
    return *((__IO uint64_t *)(address));  // Read and return the 64-bit value
}

/**
 * @brief Erase a specific flash page.
 * @note This function initiates the page erase operation for the specified flash
 *       memory address. It unlocks the flash memory, performs the erase, and
 *       then locks the flash memory again. The function returns true if the
 *       operation is successful; otherwise, it returns false.
 *
 * @param page_address The address of the page to be erased.
 * @retval Success status (true if successful, false otherwise).
 */
static bool my_flash_earse_pages(uint32_t page_address)
{
    uint32_t page_error = 0;          // Variable to hold error information if the erase fails
    FLASH_EraseInitTypeDef my_flash;  // Structure for flash erase initialization

    my_flash.TypeErase = FLASH_TYPEERASE_PAGES;   // Specify that we are erasing pages
    my_flash.Page      = get_page(page_address);  // Get the page number based on the address
    my_flash.NbPages   = 1;                       // Specify that we want to erase one page

    HAL_FLASH_Unlock();                                                    // Unlock the flash memory for write access
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&my_flash, &page_error);  // Perform the erase operation
    HAL_FLASH_Lock();                                                      // Lock the flash memory again

    // Return true if the erase was successful, false otherwise
    return (status == HAL_OK);
}

/**
 * @brief Write a double word to the specified memory address.
 * @note This function unlocks the flash memory, programs a double word at
 *       the given address, and then locks the flash memory again.
 *
 * @param address The memory address to which the data will be written.
 * @param data The 64-bit data to be written.
 * @retval true if the write operation is successful, false otherwise.
 */
static bool my_flash_write_double_word(uint32_t address, uint64_t data)
{
    HAL_FLASH_Unlock();  // Unlock the flash memory for writing
    HAL_StatusTypeDef status =
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data);  // Program the double word
    HAL_FLASH_Lock();                                                    // Lock the flash memory again

    return (status == HAL_OK);  // Return success status
}

void flash_init(void)
{
    // 从Flash读取第60页内容到buffer
    get_page60_buffer();

#ifdef FLASH_DEBUG
    printf("FLASH_DEBUG: Initial page60_buffer contents:\n");
    for (uint8_t i = 0; i < 16; i++) {
        printf("  page60_buffer[%2u] = 0x%02X\n", i, page60_buffer[i]);
    }
#endif
    // 未初始化需要初始化
    if (page60_buffer[0] != 0x11) {
        page60_buffer[0] = 0x11;
        for (uint8_t idx = 1; idx < 16; idx++) {
            if (idx != MB_COIL_INT_5V_MAIN_POWER && page60_buffer[idx] == 0xFF) {
                page60_buffer[idx] = 1;
            }
        }
        // 写回Flash
        set_page60_buffer();
#ifdef FLASH_DEBUG
        printf("FLASH_DEBUG: Updated page60_buffer contents:\n");
        for (uint8_t i = 0; i < 16; i++) {
            printf("  page60_buffer[%2u] = 0x%02X\n", i, page60_buffer[i]);
        }
#endif
    }

    // 从Flash读取第61页内容到buffer
    get_page61_buffer();
#ifdef FLASH_DEBUG
    printf("FLASH_DEBUG: Initial page61_buffer contents:\n");
    for (uint8_t i = 0; i < 8; i++) {
        printf("  page60_buffer[%2u] = 0x%04X\n", i, page61_buffer[i]);
    }
#endif
    // 未初始化，初始化
    if (page61_buffer[0] != 0x1111) {
        page61_buffer[0] = 0x1111;
        page61_buffer[1] = SYSTEM_POWER_ON_TIMER_MIN;
        page61_buffer[2] = FAN_PWM_DEFINE;
        page61_buffer[3] = VDD_CORR_DEFAULT;
        page61_buffer[4] = (uint16_t)((IP_ADDR_DEFAULT >> 16) & 0xFFFF);
        page61_buffer[5] = (uint16_t)(IP_ADDR_DEFAULT & 0xFFFF);
        page61_buffer[6] = RGB_DEFAULT_NUM;
        page61_buffer[7] = RGB_MODE_ARM_MAP;
        // 写回Flash
        set_page61_buffer();
#ifdef FLASH_DEBUG
        printf("FLASH_DEBUG: Updated page60_buffer contents:\n");
        for (uint8_t i = 0; i < 8; i++) {
            printf("  page61_buffer[%2u] = 0x%04X\n", i, page61_buffer[i]);
        }
#endif
    }
}

bool set_page60_buffer(void)
{
    uint64_t temp1    = my_flash_read_double_word(STM32G4xx_FLASH_PAGE60_STARTADDR);
    uint64_t temp2    = my_flash_read_double_word(STM32G4xx_FLASH_PAGE60_STARTADDR + 8);
    bool write_false1 = false;
    bool write_false2 = false;
    __disable_irq();

    for (int i = 0; i < 8; i++) {
        set_byte_in_uint64(&temp1, i, page60_buffer[i]);
    }

    for (int i = 0; i < 8; i++) {
        set_byte_in_uint64(&temp2, i, page60_buffer[i + 8]);
    }

    my_flash_earse_pages(STM32G4xx_FLASH_PAGE60_STARTADDR);
    write_false1 = my_flash_write_double_word(STM32G4xx_FLASH_PAGE60_STARTADDR, temp1);
    write_false2 = my_flash_write_double_word(STM32G4xx_FLASH_PAGE60_STARTADDR + 8, temp2);
    __enable_irq();
    if (write_false1 && write_false2) {
        return true;
    }
    return false;
}

bool get_page60_buffer(void)
{
    uint64_t temp1 = my_flash_read_double_word(STM32G4xx_FLASH_PAGE60_STARTADDR);
    uint64_t temp2 = my_flash_read_double_word(STM32G4xx_FLASH_PAGE60_STARTADDR + 8);
    for (int i = 0; i < 8; i++) {
        page60_buffer[i] = get_byte_in_uint64(temp1, i);
    }
    for (int i = 0; i < 8; i++) {
        page60_buffer[i + 8] = get_byte_in_uint64(temp2, i);
    }
    return true;
}

bool set_page61_buffer(void)
{
    uint64_t temp1    = my_flash_read_double_word(STM32G4xx_FLASH_PAGE61_STARTADDR);
    uint64_t temp2    = my_flash_read_double_word(STM32G4xx_FLASH_PAGE61_STARTADDR + 8);
    bool write_false1 = false;
    bool write_false2 = false;
    __disable_irq();
    for (int i = 0; i < 4; i++) {
        set_2byte_in_uint64(&temp1, i, page61_buffer[i]);
    }
    for (int i = 0; i < 4; i++) {
        set_2byte_in_uint64(&temp2, i, page61_buffer[i + 4]);
    }
    my_flash_earse_pages(STM32G4xx_FLASH_PAGE61_STARTADDR);
    write_false1 = my_flash_write_double_word(STM32G4xx_FLASH_PAGE61_STARTADDR, temp1);
    write_false2 = my_flash_write_double_word(STM32G4xx_FLASH_PAGE61_STARTADDR + 8, temp2);
    __enable_irq();
    if (write_false1 && write_false2) {
        return true;
    }
    return false;
}
bool get_page61_buffer(void)
{
    uint64_t temp1 = my_flash_read_double_word(STM32G4xx_FLASH_PAGE61_STARTADDR);
    uint64_t temp2 = my_flash_read_double_word(STM32G4xx_FLASH_PAGE61_STARTADDR + 8);
    for (int i = 0; i < 4; i++) {
        page61_buffer[i] = get_2byte_in_uint64(temp1, i);
    }
    for (int i = 0; i < 4; i++) {
        page61_buffer[i + 4] = get_2byte_in_uint64(temp2, i);
    }
    return true;
}

void set_page60_update_flag(bool flag)
{
    page60_update_flag = flag;
}

void set_page61_update_flag(bool flag)
{
    page61_update_flag = flag;
}

void flash_update(void)
{
    HAL_Delay(5);  // 等待数据发送完成

    if (page60_update_flag) {
#ifdef FLASH_DEBUG
        printf("FLASH_DEBUG: Updated page60_buffer contents:\n");
#endif
        page60_update_flag = false;
        for (uint8_t i = 1; i < 16; i++) {
            if (i != MB_COIL_INT_5V_MAIN_POWER) {
                page60_buffer[i] = g_mb_coils[i];
            }
        }
        set_page60_buffer();
    }
    if (page61_update_flag) {
#ifdef FLASH_DEBUG
        printf("FLASH_DEBUG: Updated page61_buffer contents:\n");
#endif
        page61_update_flag = false;
        page61_buffer[1]   = g_mb_holding_regs[MB_HOLD_REG_MAIN_POWER_DELAY];
        page61_buffer[2]   = g_mb_holding_regs[MB_HOLD_REG_FAN_SPEED_PERCENT];
        page61_buffer[3]   = g_mb_holding_regs[MB_HOLD_REG_VDD_CPU_VOLTAGE];
        page61_buffer[4]   = g_mb_holding_regs[MB_HOLD_REG_HOST_IP_HIGH];
        page61_buffer[5]   = g_mb_holding_regs[MB_HOLD_REG_HOST_IP_LOW];
        page61_buffer[6]   = g_mb_holding_regs[MB_HOLD_REG_RGB_COUNT];
        page61_buffer[7]   = g_mb_holding_regs[MB_HOLD_REG_RGB_MODE];
        set_page61_buffer();
    }
}
