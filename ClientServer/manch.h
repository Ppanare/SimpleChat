//была задумка кодировать сообщение но честно говоря руки не дошли
namespace manch {
	std::string Encode(const std::string& input) {
		std::string encoded;

		for (char c : input) {
			for (int i = 7; i >= 0; --i) {
				encoded += (c & (1 << i)) ? "10" : "01";
			}
		}
		return encoded;
	}

	std::string Decode(const std::string& encoded) {
		std::string decoded;
		for (size_t i = 0; i < encoded.length(); i += 2) {
			if (encoded[i] == '1' && encoded[i + 1] == '0') {
				decoded += '1';
			}
			else if (encoded[i] == '0' && encoded[i + 1] == '1') {
				decoded += '0';
			}
			else {
				std::cerr << "Panic! errors " << encoded.substr(i, 2) << std::endl;
				return "";
			}
		}
		return decoded;
	}
}