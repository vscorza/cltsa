----------------------------------------------------------------------------
-- FSM_Sync_Control
--
-- Esta máquina busca el sincronismo de 5 TSPs. Para eso, busca
-- 5 bytes de sincronismo("0x47") espaciados en 204 bytes entre ellos
----------------------------------------------------------------------------

library ieee;
library comun;
use ieee.std_logic_1164.all;
use comun.funciones.all;

entity FSM_Sync_Control is
  port (
    clk_i         : in  std_logic;
    rst_i         : in  std_logic;
-- Señales de entrada
    we_i          : in  std_logic;
    data_i        : in  std_logic_vector (7 downto 0);
    count_i       : in  std_logic_vector (7 downto 0);
-- Señales de salida
    locked_o      : out std_logic;
    reset_count_o : out std_logic;
    count_en_o    : out std_logic
    );
end entity;

architecture FSM_Sync_Control_arch of FSM_Sync_Control is

  type estado is (idle, chk_1, chk_2, chk_3, chk_4, locked);
  signal present_state : estado;
  signal next_state    : estado;

  signal cmd : std_logic;

begin

-- Actualización del estado
  process (clk_i, rst_i)
  begin
    if rising_edge(clk_i) then
      if (rst_i = '1') then
        present_state <= idle;
      elsif(we_i = '1') then
        present_state <= next_state;
      end if;
    end if;
  end process;

-- Lógica de estado siguiente
  process (present_state, count_i, data_i)
  begin
    case present_state is
      
      when idle =>
        if data_i = x"47" then
          next_state <= chk_1;
        else
          next_state <= present_state;
        end if;
        
      when chk_1 =>  -- Se encuentra un candidato para iniciar la sincronización
        if count_i = x"CB" then
          if data_i = x"47" then
            next_state <= chk_2;
                     -- Si luego de 204 bytes se encuentra otro byte,
                     -- se aumenta el estado de sincronismo
          else
            next_state <= idle;
                     -- Si luego de 204 bytes NO se encuentra otro byte de sincronismo
                     -- se regresa la sincronización a 0
          end if;
        else
          next_state <= present_state;
        end if;

        
      when chk_2 =>  -- Se encuentra un candidato para iniciar la sincronización
        if count_i = x"CB" then
          if data_i = x"47" then
            next_state <= chk_3;
                     -- Si luego de 204 bytes se encuentra otro byte,
                     -- se aumenta el estado de sincronismo
          else
            next_state <= idle;
                     -- Si luego de 204 bytes NO se encuentra otro byte de sincronismo
                     -- se regresa la sincronización a 0
          end if;
        else
          next_state <= present_state;
        end if;

        
      when chk_3 =>  -- Se encuentra un candidato para iniciar la sincronización
        if count_i = x"CB" then
          if data_i = x"47" then
            next_state <= chk_4;
                     -- Si luego de 204 bytes se encuentra otro byte,
                     -- se aumenta el estado de sincronismo
          else
            next_state <= idle;
                     -- Si luego de 204 bytes NO se encuentra otro byte de sincronismo
                     -- se regresa la sincronización a 0
          end if;
        else
          next_state <= present_state;
        end if;

        
      when chk_4 =>  -- Se encuentra un candidato para iniciar la sincronización
        if count_i = x"CB" then
          if data_i = x"47" then
            next_state <= locked;
                     -- Si luego de 204 bytes se encuentra otro byte,
                     -- se aumenta el estado de sincronismo
          else
            next_state <= idle;
                     -- Si luego de 204 bytes NO se encuentra otro byte de sincronismo
                     -- se regresa la sincronización a 0
          end if;
        else
          next_state <= present_state;
        end if;

      when locked =>
        if count_i = x"CB" then
          if data_i = x"47" then
            next_state <= present_state;
                     -- Si luego de 204 bytes se encuentra otro byte,
                     -- se mantiene sincronismo
          else
            next_state <= idle;
                     -- Si luego de 204 bytes NO se encuentra otro byte de sincronismo
                     -- se pierde sincronismo y se regresa la sincronización a 0
          end if;
        else
          next_state <= present_state;
        end if;
        
      when others =>
        next_state <= idle;
        
    end case;
  end process;

-- Logica de salida
  process (present_state)
  begin
    case present_state is
      
      when idle =>
        locked_o      <= '0';
        reset_count_o <= '1';
        count_en_o    <= '0';
      when chk_1 =>
        locked_o      <= '0';
        reset_count_o <= '0';
        count_en_o    <= '1';
      when chk_2 =>
        locked_o      <= '0';
        reset_count_o <= '0';
        count_en_o    <= '1';
      when chk_3 =>
        locked_o      <= '0';
        reset_count_o <= '0';
        count_en_o    <= '1';
      when chk_4 =>
        locked_o      <= '0';
        reset_count_o <= '0';
        count_en_o    <= '1';
      when locked =>
        locked_o      <= '1';
        reset_count_o <= '0';
        count_en_o    <= '1';
      when others =>
        locked_o      <= '0';
        reset_count_o <= '1';
        count_en_o    <= '0';
    end case;

  end process;

end architecture;

