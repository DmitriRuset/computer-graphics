#pragma once

#include <shooter/projectiles.hpp>
#include <shooter/colored_figures.hpp>

#include <fstream>
#include <string>

class SaveLoad {
public:
    void SaveScene(Projectiles& projectiles, ColoredFigures& enemies, int shot_enemies) {
        std::fstream save_file;
        save_file.open(scene_file, std::ios::out);
        save_file.clear();
        // Save Enemies, Fireballs
        save_file << projectiles.GetItems().size() << '\n';
        for (auto& fireball: projectiles.GetItems()) {
            save_file << fireball->GetPos().x << " " <<
                      fireball->GetPos().y << " " << fireball->GetPos().z << '\n';

            save_file << fireball->GetSpeed().x << " " <<
                      fireball->GetSpeed().y << " " << fireball->GetSpeed().z << '\n';
        }

        save_file << enemies.GetItems().size() << '\n';
        for (auto& enemy: enemies.GetItems()) {
            save_file << enemy->GetPos().x << " " <<
                      enemy->GetPos().y << " " <<
                      enemy->GetPos().z << '\n';

            save_file << enemy->GetRot().x << " " <<
                        enemy->GetRot().y << " " <<
                        enemy->GetRot().z << " " <<
                        enemy->GetRot().w << '\n';
        }

        save_file << shot_enemies << '\n';

        save_file.close();
    }

    void LoadScene(Projectiles& projectiles, ColoredFigures& enemies, int& shot_enemies) {
        std::ifstream load_file(scene_file);

        if (load_file.is_open()) {
            projectiles.Clear();
            enemies.Clear();

            int cnt_fireballs;
            load_file >> cnt_fireballs;

            for (int i = 0; i < cnt_fireballs; ++i) {
                glm::vec3 pos;
                load_file >> pos.x >> pos.y >> pos.z;

                glm::vec3 move_speed;
                load_file >> move_speed.x >> move_speed.y >> move_speed.z;

                Fireball fireball{pos, move_speed};
                projectiles.AddFireball(fireball);
            }

            int cnt_enemies;
            load_file >> cnt_enemies;

            for (int i = 0; i < cnt_enemies; ++i) {
                glm::vec3 pos;
                load_file >> pos.x >> pos.y >> pos.z;

                glm::quat rot;
                load_file >> rot.x >> rot.y >> rot.z >> rot.w;

                Enemy enemy{pos, rot};
                enemies.AddEnemy(enemy);
            }

            load_file >> shot_enemies;
        }

        load_file.close();
    }
private:
    const std::string scene_file = "scene.txt";
};
