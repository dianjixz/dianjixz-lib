```bash
wget https://m5share.oss-cn-shenzhen.aliyuncs.com/ax630c_llm_project/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tar.gz?OSSAccessKeyId=LTAI5tSGrycCku6R4TQkroEc&Expires=37727406013&Signature=U9YN9Tp6zrIhNlL%2FfjcO0xRQTzk%3D
sudo tar zxvf gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tar.gz -C /opt


sudo apt install python3 python3-pip libffi-dev
pip3 install parse scons requests 


cd M5Stack_LLM_Framework/examples/ax_llm_firework
scons distclean
scons -j22





```