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
`GameObject` là class cha của tất cả các thực thể có mặt trong game. Bất kể là Mario, Goomba, Đồng xu hay Đường ống, tất cả đều phải kế thừa từ class này.
### a. Thuộc tính 
Mỗi vật thể khi sinh ra đều mang trong mình các thông số cơ bản:
- Tọa độ (x, y): Vị trí của vật thể trên thế giới (World Space).
- Vận tốc (vx, vy): Tốc độ di chuyển theo trục X và Y. (Quy ước: vy > 0 là đang rớt xuống, vy < 0 là nhảy lên).
- Hướng quay mặt (nx): 1 là hướng sang phải, -1 là hướng sang trái. Rất quan trọng để lật (flip) Sprite khi vẽ.
- Trạng thái (state): Con số định nghĩa hành động hiện tại (ví dụ: đang đi, đang nhảy, đã chết). Hàm SetState() thường được ghi đè để reset lại vận tốc khi đổi trạng thái.
- Cờ xóa (isDeleted): Khi bằng true, Scene sẽ tự động quét và thu hồi vùng nhớ (delete) vật thể này ở cuối vòng lặp Update. Tuyệt đối không tự gọi lệnh delete this trong code.

### b. Phương thức nền tảng
- Constructor: khởi tạo những giá trị cho lớp, bắt buộc phải có 2 biến x, y ban đầu cho tọa độ. Giống Start() trong unity
- Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects): Cập nhật logic của vật thể theo thời gian trong game (dt).
- Render(): Nếu Update() cập nhật logic thì Render là cập nhật hình ảnh sau mỗi khung hình.
- GetBoundingBox(float &left, float &top, float &right, float &bottom): Trả về 4 cạnh của một hình chữ nhật bao quanh vật thể, là Collider của mọi vật để để nhận diện nhau.
### c. Các phương thức khác
- IsCollidable(): Có là đối tượng tham gia xét va chạm không, mặc định là 1, chỉ những vật như đám mây trang trí, quái vật đã chết thì 0
- IsBlocking(): Chặn vật khác có bật Collidable
- IsDirectionColliable(float nx, float ny): Vật thể platform check va chạm 1 chiều.
- RenderBoundingBox(): Vẽ hình vuông đỏ để xem collider thực tế. Dùng trong hàm Render để vẽ.

## 3. Quy trình tạo một Vật thể (Object) mới
### Bước 1: Cắt ảnh và Định danh (Data/Assets)

- Mở file Texture gốc. Lấy tọa độ left, top, right, bottom của các frame hình đồng xu.
- Định nghĩa ObjectID, SpriteID và AnimationID vào file AssetIDs.h theo đúng chuẩn quy hoạch.
- Viết tọa độ đã cắt vào file cấu hình của vật thể (Ví dụ: assets.txt).
	- [SPRITES] có cấu trúc trên một dòng là
		- SpriteID: định danh sprite
		- Left Top Right Bottom: các đường cắt quanh hình chữ nhật
		- TextureID: ảnh cần cắt
	- [ANIMATIONS] có cấu trúc trên một dòng là
		- AnimtionID: định danh animation
		- SpriteID_1 Time_1: Ảnh 1 tại thời điểm 1
		- SpriteID_n Time_n: Ảnh n tại thời điểm n

### Bước 2: Viết Class Logic (C++)

- Tạo class kế thừa từ GameObject.
- Cài đặt kích thước GetBoundingBox.
- Cài đặt logic trong Update()
- Cài đặt Render() để vẽ Animation.

### Bước 3: Khai báo vào Cấu trúc Map
- Mở file Map .txt cần xem trước, xem hướng dẫn trong README.md
- Thêm một dòng mới tương ứng với ObjectID của Đồng xu, kèm theo tọa độ X, Y muốn thả nó vào Map.

### Bước 4: Nhúng vào Hệ thống (PlayScene.cpp)
- Mở hàm _ParseSection_OBJECTS trong file `PlayScene.cpp`.
- Thêm một case mới vào lệnh `switch (object_type)`
	> ```
	> case OBJECT_TYPE_...: obj = new ...(x, y); break;
	>```
- Compile và test thử trong game.
