"""
Базовый класс создаёт таблицу (название | поле ввода) в соответствии с переданными названиями.
На его оснве сделан ещё один класс добавляющий к нему заголовок.

Есть заготовка для работы с COM портом, её нужно дописать для корректной работы

Все это компануется в один класс, имеющий два раздела для вывода и ввода данных соответственно.
Он является прослойкой между отображением информации на экране и работой с портом.

Раз в заданное время пробует получить данные из порта и если они есть обновляет значения с датчиков
По нажатию кнопки Отправить данные отправлятся в порт, если он выбран. При не выбранном порте, выдается 
сообщение с ошибкой
"""
import serial
import glob
import sys

from tkinter import Tk, Label, Entry, Frame, messagebox
from tkinter import ttk


class Terminal(object):
    """Класс для общения с последовательным портом."""
    def __init__(self, baudrate=9600):
        """Инициализация порта"""
        self.baudrate = baudrate
        self.values = []
        self.ser = serial.Serial(baudrate=9600)

    def __del__(self):
        if self.ser.is_open:
            self.ser.close()

    @staticmethod
    def get_serial_ports():
        """ Возвращает список доступных портов."""
        if sys.platform.startswith('win'):
            # Генерируем список возможных портов для Windows
            all_ports = ['COM{}'.format(i + 1) for i in range(256)]
        elif sys.platform.startswith('linux'):
            # Генерируем список возможных портов для Linux
            all_ports = glob.glob('/dev/tty[A-Za-z]*')
        else:
            raise EnvironmentError('Unsupported platform')

        result = []
        for port in all_ports:
            # Пробуем открыть порт, если открывается то добавляем-запоминаем
            try:
                s = serial.Serial(port=port)
                s.close()
                result.append(port)
            except (OSError, serial.SerialException):
                pass
        return result

    def send_data(self, port, data):
        """Отправка данных в порт."""
        if self.ser.port != port:
            self.ser.port = port  # 'Serial' object is not callable

        if self.ser.is_open:
            self.values = data
            for value in self.values:
                if value == self.values[-1]:
                    self.ser.write(value)
                    self.ser.write(b"\n")

                else:
                    self.ser.write(value)

        else:
            self.ser.open()
            self.values = data
            for value in self.values:
                if value == self.values[-1]:
                    self.ser.write(value)
                    self.ser.write(b"\n")
                else:
                    self.ser.write(value)

    def get_data(self, port):
        """Чтение данных из порта."""
        if self.ser.port != port:
            self.ser.port = port  # 'Serial' object is not callable

        if self.ser.is_open:
            self.values = self.ser.readline().decode().split(" ")
            print(self.values)
            return self.values

        else:
            self.ser.open()
            self.values = [self.ser.readline().decode().split(" ")]
            return self.values


class BaseTable(Frame):
    """"Элемент для генерации таблицы значений"""
    def __init__(self, names, master=None, label_length=None):
        """
        Принимает список названий полей. Генерирует таблицу.
        :param names: список названий полей вывода информации
        """
        Frame.__init__(self, master=master)

        self.entries = []
        for name in names:
            Label(master=self, text=name, anchor='w', width=label_length).grid(row=names.index(name),
                                                                               column=0, sticky='w')
            self.entries.append(Entry(master=self, width=7))
            self.entries[-1].grid(row=names.index(name), column=1)

    def get(self):
        """Возвращает список занчений из полей ввода"""
        return [ent.get() for ent in self.entries]

    def set_values(self, values):
        """Задает значение полям ввода"""
        for i in range(len(values)):
            # Запомним текущее состояние поля
            current_state = self.entries[i]['state']
            # Сделаем доступным для ввода
            self.entries[i]['state'] = 'normal'
            # Удаляем текущее значение
            self.entries[i].delete(0, 'end')
            self.entries[i].insert(0, values[i])
            self.entries[i]['state'] = current_state

    def entries_state(self, state='normal'):
        """Задает возможность ввода данных в поля.
           NORMAL - доступны для ввода
           DISABLED -- недоступны для ввода"""
        for ent in self.entries:
            ent.config(state=state)


class InfoFrame(Frame):
    """Элемент для вывода информации в виде таблицы с заголовком"""
    def __init__(self, title,  names, label_length=None):
        Frame.__init__(self)
        Label(master=self, text=title).pack()
        self.table = BaseTable(names, self, label_length=label_length)
        self.table.pack()

    def get(self):
        return self.table.get()

    def update(self, values):
        self.table.set_values(values)

    def state(self, state='normal'):
        self.table.entries_state(state)


class GreenWindow(object):

    def __init__(self):
        # Подключаем работу с COM портом
        self.term = Terminal()

        self.info_title = 'Показания датчиков:'
        self.info_names = ['Время:', 'Влажность:', 'Освещенность:', 'Наличие воды']
        self.setting_title = 'Задать значения:'
        self.setting_names = ['Минимальная влажность:', 'Длительность дня:']
        # Задаем период обновления данных
        self.refresh_time = 1000

        self.root = Tk()
        self.root.title("Greenhouse")
        self.create_main_window()

    def create_main_window(self):
        """Размещаем поля для состояния датчиков и задания настроек"""
        # Возможно более правильно перенети объявлеие переменных от сюда в __init__
        names = self.setting_names + self.info_names
        # Узнаем размер самой длинной надписи для выравнивания надписей по ширине
        label_len = max(map(len, names))

        self.info = InfoFrame(self.info_title, self.info_names, label_len)
        # Делаем поля недоступными для ввода данных
        self.info.state('disabled')
        self.info.pack()

        self.settings = InfoFrame(self.setting_title, self.setting_names, label_len)
        self.settings.pack()

        # Создаем панель для размещения кнопки и списка доступных портов
        dashboard = Frame()
        dashboard.pack()
        # Создаем выпадающий список с доступными последовательными портами
        self.ports_box = ttk.Combobox(master=dashboard, values=self.term.get_serial_ports())
        self.ports_box.config(state='readonly', width=6)
        self.ports_box.pack(side='left')

        ttk.Button(master=dashboard, text='Задать значения', command=lambda: self.send_settings()).pack(side='right')

        # Создаем событие обновления
        self.update_info()
        self.root.mainloop()

    def send_settings(self):
        """Забирает данные из полей настроек, преобразует в бинарые и передает в последовательный порт"""
        values = [val.encode() for val in self.settings.get()]
        # Получаем значение выбранного порта
        port = self.ports_box.get()
        if port:
            self.term.send_data(port, values)
        else:
            messagebox.showerror('Serial port error', 'Выберите доступный порт!')

    def update_info(self):
        """Опрашивем порт и при наличии новых данных обновляем значения"""
        port = self.ports_box.get()
        values = []

        if port:
            values = self.term.get_data(port)

        if values:
            if len(values) == len(self.info_names):
                self.info.update(values)
        # Создаем постоянно повторяющиеся действие
        self.root.after(self.refresh_time, self.update_info)

# ser = serial.Serial(port="COM4", baudrate=9600)
d = GreenWindow()
