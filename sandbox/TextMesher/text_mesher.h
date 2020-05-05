#ifndef EASY3D_TEXT_MESHER_H
#define EASY3D_TEXT_MESHER_H

#include <string>

#include <easy3d/core/types.h>


namespace easy3d {

    class SurfaceMesh;

    // TODO: implement the vectorizer in Easy3D;
    // TODO: make TextMesher a formal part of Easy3D.
    class TextMesher {
    public:
        /**
         * @brief Create an instance of TextMesher.
         * @param font_file The full path name to a TrueType font file (normally with an "ttf" extension).
         * @param font_height The height of the font.
         * @note The font will be used in all subsequent generation procedure until the next call to set_font().
         */
        TextMesher(const std::string &font_file, int font_height = 48);

        ~TextMesher();

        /**
         * @brief Change the font.
         * @param font_file The full path name to a TrueType font file (normally with an "ttf" extension).
         * @param font_height The height of the font.
         * @note The font will be used in all subsequent generation procedure until the next call to set_font().
         */
        void set_font(const std::string &font_file, int font_height);

        /**
         * @brief Generate a 3D surface mesh of a text.
         * @param text The input text.
         * @param x The x-coordinate of the starting position.
         * @param y The y-coordinate of the starting position.
         * @param extrude The height (in the Z direction) of the 3D model.
         * @return The generated triangular surface mesh.
         */
        SurfaceMesh *generate(const std::string &text, float x, float y, float extrude = 16);

        /**
         * @brief Generate 3D surface representation of a text and append the surface to an existing mesh.
         * @param text The input text.
         * @param x The x-coordinate of the starting position.
         * @param y The y-coordinate of the starting position.
         * @param extrude The height (in the Z direction) of the 3D model.
         * @param True on success and false on failure.
         */
        bool generate(SurfaceMesh* mesh, const std::string &text, float x, float y, float extrude = 16);

    public:
        /**
         * A contour is a closed polygon and it has an orientation (clockwise or counter-clockwise)
         */
        struct Contour : Polygon2 {
            Contour() : clockwise(false) {}
            Contour(std::size_t size) : Polygon2(size), clockwise(false) {}
            bool clockwise;
        };

        /**
         * CharContour represents the contours of a character, which may contain multiple contours.
         */
        struct CharContour : std::vector<Contour> {
            char character;
        };

        /**
         * @brief Generate contours for a text.
         * @param text The input text.
         * @param x The x-coordinate of the starting position.
         * @param y The y-coordinate of the starting position.
         * @param contours The contours of the text. The generated contours are simply appended to his variable.
         */
        void generate_contours(const std::string &text, float x, float y, std::vector<CharContour> &contours);

        /**
         * @brief Generate contours for a single character.
         * @param character The input character.
         * @param x The x-coordinate of the starting position. In return, the new value for the subsequent character.
         * @param y The y-coordinate of the starting position. In return, the new value for the subsequent character.
         * @return The contours for this character.
         */
        CharContour generate_contours(char character, float& x, float& y);

    private:
        void cleanup();

    private:
        void *font_library_;
        void *font_face_;
        bool ready_;

        std::string font_file_;
        int font_height_;

        // Controls the smoothness of the curved corners. A greater value results in a smooth transitions but more
        // vertices. Suggested value is 4.
        unsigned short bezier_steps_;

        unsigned int prev_char_index_;
        unsigned int cur_char_index_;
        signed long prev_rsb_delta_;;
    };
}


#endif  // EASY3D_TEXT_MESHER_H
