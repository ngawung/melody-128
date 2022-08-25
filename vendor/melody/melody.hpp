#pragma once
#include <cstdint>

namespace melody {
	class Scene {
		public:
			Scene();
			virtual ~Scene();
			
			virtual void initialize();
			virtual void update();
	};

	class App {
		public:
			App();
			~App();

			void start(Scene* scene);
			void update();

			// GET & SET

			void set_scene(Scene* scene);
			Scene* get_scene();

			static App* get_instance();

		private:
			static App* _instance;
			
			Scene* _currentScene;
	};

	struct Matrix {
		float a;
		float b;
		float c;
		float d;
		float tx;
		float ty;

		void identity();
		void translate(float dx, float dy);
		void scale(float sx, float sy);
		void rotate(float theta);
	};

	class Random {
        public:
            Random();
            void setSeed(uint32_t w);
            void setSeed(uint32_t w, uint32_t z);
            void setSeedFromTime();

            uint32_t MWC();
            float UNI();
            float range(uint32_t min, uint32_t max);
            float floorRange(uint32_t min, uint32_t max);

            float xorshift_uni(uint32_t x);
            float xorshift64_uni(uint64_t x);
            uint32_t xorshift(uint32_t x);
            uint64_t xorshift64(uint64_t x);

        private:
            uint32_t m_w;
            uint32_t m_z;
            float inv_uni = 2.3283064e-10;
            float inv_uni64 = 5.4210109e-20;
    };
}