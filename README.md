cw-bmp
======
Вот вроде как есть рабочий вариант. Работает с 24бит изображениями.
Есть всякие мелкие недопиленности:
* Отсутствие справки. Запускать а [--filter_name1] [--filter_name2] <source.bmp>
  Сейчас есть следующие фильтры: negative,rgb100,diagonal,frame
  Из них осмыслению поддаются первые два. Остальные творят трэш, но это ж тоже обработка.
  Постараюсь привести их в порядок, добавить парочку.
* Не ругается и не проверяет неправильный ввод. Если введено неправильно, просто будет копирован исходный файл.
  так проще отслеживать корректность ввода. "Баг сам себя лечит".
* Не реализована, но предполагалась идея применения нескольких фильтров в заданной последовательности.
  Пока что просто записывается очерёдность ввода параметра.
  Кстати, возникла, идея возможности повторного применения фильтра в одной последовательности.
  Пока недопустимо из-за организации. Оно надо, или это неправильно, когда один параметр пару раз вызывается?
* Считывание только 24битных  изображений 
* Непродуманная организация работы-хранения переданных параметров. Требует переосмысления.
  
  
Что есть:
* Программа, корректно считывающая структуру 24битного BMP изображения.
  С пропуском не несущих смысловой нагрузки участков. 
* Простейшая обработка изображения на основе сетки RGB
* Некий каркас для дальнейшего развития(часть нуждается в замене, что на данном этапе вполне логично)
* Необходимость разбиения на несколько файлов...