Tín hiệu âm thanh xuất ra từ chân DAC (PA5) của vi điều khiển là tín hiệu tương tự (Analog) nhưng có dòng điện cực kỳ nhỏ. Hơn nữa, DAC của STM32 xuất dải điện áp từ 0V đến 3.3V (tín hiệu đơn cực - Unipolar), nghĩa là ở trạng thái im lặng, điện áp luôn neo ở mức khoảng 1.65V (chứ không phải 0V).

Nếu bạn cắm trực tiếp một chiếc loa vào chân PA5, âm thanh sẽ rất bé, loa bị nóng do dòng DC chạy qua liên tục, hoặc thậm chí làm hỏng chân IO của vi điều khiển. Để chạy đúng chuẩn kỹ thuật mạch điện nhúng, bạn cần các phần cứng sau:

1. Danh sách phần cứng cần chuẩn bị
Mạch khuếch đại âm thanh (Audio Amplifier Module): Phổ biến, rẻ và dễ dùng nhất cho các dự án môn học là mạch PAM8403 (mạch khuếch đại Class D, công suất 3W, chạy nguồn 5V). Bạn cũng có thể dùng mạch LM386.

Loa mini (Speaker): Một chiếc loa nhỏ trở kháng 4-Ohm (công suất 3W) hoặc 8-Ohm (0.5W - 1W).

Tụ điện hóa (Coupling Capacitor): 1 tụ điện có giá trị từ 10µF đến 100µF. Linh kiện này cực kỳ quan trọng trong kỹ thuật xử lý tín hiệu để chặn dòng một chiều (DC Block), bảo vệ loa và mạch.

Dây cắm (Jumper wires): Dây đực-cái, cái-cái để nối bo mạch.

2. Sơ đồ và Cách lắp đặt chi tiết (Dùng mạch PAM8403)
Bạn thực hiện nối dây theo 3 bước sau:

Bước 1: Nối tín hiệu qua tụ lọc (DC Blocking)

Nhìn trên bo mạch STM32F429I-DISCO, tìm chân PA5.

Lấy tụ điện hóa, nối chân PA5 vào chân Dương (+) của tụ (chân dài hơn).

Nối chân Âm (-) của tụ (chân ngắn hơn, có vạch sọc trắng trên thân) vào chân L (hoặc R) ở cụm đầu vào tín hiệu (Audio In) của mạch PAM8403.

Lý do: Tụ điện sẽ chặn mức điện áp 1.65V cố định (DC) từ vi điều khiển, chỉ cho phép tín hiệu dao động âm thanh (AC) đi qua mạch khuếch đại.

Bước 2: Cấp nguồn và nối Mass (GND)

Tìm chân 5V trên board STM32, nối vào chân 5V (hoặc VCC) trên mạch PAM8403.

Tìm chân GND trên board STM32, nối vào chân GND (cụm nguồn) của mạch PAM8403.

Nếu cụm đầu vào tín hiệu (Audio In) của PAM8403 có thêm một chân GND riêng, bạn cũng nối chân đó chung vào GND của board STM32 để chống nhiễu (chống tiếng ù xì).

Bước 3: Nối Loa vào ngõ ra

Lấy 2 dây của chiếc loa mini, nối vào cụm ngõ ra L+ và L- (tương ứng với ngõ vào L bạn đã cắm tụ ở Bước 1) trên mạch PAM8403. Không phân biệt chiều âm dương của bản thân cái loa, bạn cắm chiều nào loa cũng kêu.

Cách test nhanh (Nếu chưa mua kịp mạch khuếch đại)
Nếu bạn đang làm dự án mà chưa có sẵn PAM8403 và loa ngoài, bạn có thể dùng một chiếc Tai nghe (Earphone) cũ cắm cổng 3.5mm để nghe thử:

Bạn vẫn bắt buộc phải nối chân PA5 qua phần Dương (+) của tụ điện 10µF - 100µF.

Phần Âm (-) của tụ điện, bạn quấn (hoặc kẹp) vào phần mũi nhọn cùng (Tip) của jack cắm tai nghe 3.5mm.

Kẹp một dây từ GND của STM32 vào phần cuống dài nhất (Sleeve) của jack 3.5mm.

Đeo một bên tai nghe vào và bấm nút trên màn hình TouchGFX, bạn sẽ nghe thấy tiếng súng và tiếng nổ rất rõ ràng.
