# Hướng dẫn thiết kế game object

## 1. ID convention 
### Về hệ thống các ID
Framework gồm 5 hệ thống ID độc lập:
- **TextureID**: Các ảnh gốc, được load từ `game-data.txt`, được quản lý bởi `Textures::GetInstance()`.
- **SceneID**: Load từ `game-data.txt`, được quản lý bởi `SceneManager::GetInstance()`. Dùng trong load scene khi chơi và chuyển scene.
	> Khi một scene được load, nó load tất cả `[ASSETS]` và `[OBJECTS]` được dùng trong scene, với các SpriteID và AnimationID độc lập nhau.
- **SpriteID**: Lưu trên file `.txt` của vật thể (hoặc file assets của scene), được quản lý bởi `Sprites::GetInstance()`. Dùng để định nghĩa một khung ảnh tĩnh được cắt ra từ Texture gốc dựa trên tọa độ.
- **AnimationID**: Lưu trên file `.txt` của vật thể, được quản lý bởi `Animations::GetInstance()`. Là một chuỗi (mảng) chứa các SpriteID liên tiếp nhau, kết hợp với thời gian delay để tạo thành chuyển động.
- **ObjectID** (Object Type): Định nghĩa họ/loại vật thể. Dùng làm cờ nhận diện trong phần `[OBJECTS]` của file scene để Engine biết cần khởi tạo thực thể nào (ví dụ: `0` là Mario, `10` là Goomba) và hỗ trợ phân cụm logic khi xử lý va chạm (Collision).

Các ID không được trùng nhau trong nội bộ hệ thống, nhưng giữa các hệ thống không ảnh hưởng lẫn nhau. Như có thể có ID 0 cho scene, gameobject,...

---

### Phân loại vật thể (Category)
Trước khi phân hoạch các ID, cùng phân loại các loại vật thể, để sau này nhìn vào một ID còn biết nó là vật thể gì. Những quy tắc dưới đây dùng cho 3 hệ thống cuối là **SpriteID**, **AnimationID** và **ObjectID** do gắn liền với vật thể game. Scene và Texture khá ít nên không cần
1. Player: nhân vật được người chơi điều khiển
2. Enemies: quái vật
	- Goomba
	- Kooba
	- Piranha Plant
	- ...
3. Interactive Blocks: Khối tương tác và môi trường có phản ứng
	- Brick block
	- Question block
	- Wood block
	- Note block (khối nút nhạc nhảy lên)
	- Pipe: ống nước
4. Items: các vật phẩm thu thập, coins, bouncing coin, lá, nấm
5. Weapons và Effect: vũ khí, cầu lửa, và các hiệu ứng sinh ra sau này
6. Map chính: địa hình tĩnh, background
7. Platforms: cấu tạo từ nhiều khối, quy tắc đặt biệt
8. World Map: các object trên các scene giới thiệu, chọn màn chơi
9. UI, system: font chữ, âm thanh


### Quy hoạch ID

Cấu trúc định dạng chuẩn: **`CTT[FSDF]`**

- **`C`: Category.** Lấy từ 1 đến 9 tương ứng với danh sách phân loại ở mục 2.
- **`TT`: Type (Loại vật thể).** Định danh cụ thể của đối tượng bên trong đại nhóm đó (Ví dụ: Nhóm 2 là Quái, Type 201 là Goomba, Type 202 là Koopa).
- **`[FSDF]`: Form/State/Direction/Frame.** Mô tả biến thể, trạng thái hành động, hướng quay mặt và thứ tự khung hình. 4 chữ số cuối không bắt buộc, do mọi người tự phân hoạch, đây chỉ là cấu trúc của thầy.

Việc phân hoạch chi tiết bên trong thế nào do mọi người tự quyết định, nhưng đảm bảo 3 chữ cái đầu để phân biệt cho các vật thể. Ví dụ 1 quái vật có **ObjectID** là `201`, những **AnimationID** cần ít nên có thể là `2013` và `2016`, còn hình ảnh **SpriteID** thì cần nhiều nên chia thành `2011838` hoặc `2013636`.

Và vì 3 thuộc tính này có quan hệ kế thừa lẫn nhau (Object chứa nhiều Animation, Animation chứa nhiều Sprite), nên có thể đặt tên cho kế thừa một chút, như `301` là ObjectID, `3011000` là anination cơ bản, `3011234` là một sprite trong animation đó
> Như đã giải thích thì các hệ thống ID là độc lập, nên không có vấn đề gì khi một nhân vật có ObjectID là `101`, AnimationID cũng là `101` và SpriteID cũng là `101`, chỉ là đặt sao cho dễ quản lý là được.

## 2. Về class GameObject.h

