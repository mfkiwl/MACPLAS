#ifndef macplas_utilities_h
#define macplas_utilities_h

#include <deal.II/base/function_lib.h>
#include <deal.II/base/point.h>
#include <deal.II/base/tensor.h>
#include <deal.II/base/timer.h>
#include <deal.II/base/utilities.h>

#include <deal.II/lac/vector.h>

#include <array>
#include <fstream>
#include <iomanip>
#include <map>
#include <string>
#include <vector>

using namespace dealii;

// helper functions

/** Calculate the square \f$x^2\f$
 */
inline double
sqr(const double x);

/** Convert string to a vector of double
 */
inline std::vector<double>
split_string(const std::string &s, const char delimiter = ',');

/** Initialize function as \c InterpolatedTensorProductGridData if \c expression
 * ends with \c txt, \c dat or \c tsv, otherwise \c FunctionParser.
 */
inline void
initialize_function(std::unique_ptr<Function<1>> &f,
                    const std::string &           expression,
                    const std::string &           vars = "t");

/** Just like \c std::minmax, for \c Vector argument
 */
template <class T>
inline std::pair<T, T>
minmax(const Vector<T> &x);

/** Just like \c std::minmax, for \c BlockVector argument
 */
template <class T>
inline std::pair<T, T>
minmax(const BlockVector<T> &x);

/** Integrate analytically \f$\dot{x} = a + b (x-x_0)\f$
 */
inline double
dx_analytical(const double a, const double b, const double dt);

/** Prints time in seconds, enclosed in parenthesis
 */
inline std::string
format_time(const double x);

/** Same as above, for \c Timer
 */
inline std::string
format_time(const Timer &timer);

/** Names of coordinates in \c dim dimensions
 */
inline std::vector<std::string>
coordinate_names(const unsigned int dim);

/** Save data (\c Vector, \c BlockVector) to disk.
 * Internally calls \c block_write
 */
template <typename T>
inline void
write_data(const T &data, const std::string &file_name);

/** Load data (\c Vector, \c BlockVector) from disk.
 * Internally calls \c block_read
 */
template <typename T>
inline void
read_data(T &data, const std::string &file_name);

/** Write coordinates and field values at quadrature points to disk
 */
template <int dim>
inline void
output_boundary_field_at_quadrature_points(
  const DoFHandler<dim> &dh,
  FEFaceValues<dim> &    fe_face_values,
  const Vector<double> & field,
  const unsigned int     boundary_id = 0,
  const std::string &    file_name   = "values_q.txt");

/** Get point on a line segment which is closest to the given point \f$p\f$
 */
template <int dim>
inline Point<dim>
closest_segment_point(const Point<dim> &p,
                      const Point<dim> &segment_p0,
                      const Point<dim> &segment_p1);

/** Get barycentric coordinates of point \f$p\f$ of a line segment
 */
template <int dim>
inline std::array<double, 2>
barycentric_coordinates(const Point<dim> &p,
                        const Point<dim> &segment_p0,
                        const Point<dim> &segment_p1);

// helper classes

/** Class for storing and quering geometrical information of a single triangle
 */
template <int dim>
class Triangle
{
public:
  /** Set the triangle vertices
   */
  inline void
  reinit(const Point<dim> &p0, const Point<dim> &p1, const Point<dim> &p2);

  /** Get the triangle center
   */
  inline Point<dim>
  center() const;

  /** Get the triangle normal
   */
  inline Point<dim>
  normal() const;

  /** Get the triangle area
   */
  inline double
  area() const;

  /** Get the longest triangle side
   */
  inline double
  longest_side() const;

  /** Get triangle point which is closest to the given point \f$p\f$
   */
  inline Point<dim>
  closest_triangle_point(const Point<dim> &p) const;

  /** Get barycentric coordinates of the given point \f$p\f$
   */
  inline std::array<double, 3>
  barycentric_coordinates(const Point<dim> &p) const;

private:
  /** Calculate triangle normal and area in one go
   */
  inline void
  calculate_normal_and_area();

  /** Calculate signed area of another triangle.
   * The normal direction of the current triangle is taken into account to get
   * the correct sign.
   */
  inline double
  signed_area(const Point<dim> &p0,
              const Point<dim> &p1,
              const Point<dim> &p2) const;

  /** Project the given point \f$p\f$ to the triangle place
   */
  inline Point<dim>
  project_to_triangle_plane(const Point<dim> &p) const;

  /** Vertices
   */
  std::array<Point<dim>, 3> m_points;

  /** Normal
   */
  Point<dim> m_normal;

  /** Center
   */
  Point<dim> m_center;

  /** Area
   */
  double m_area;

  /** Longest side
   */
  double m_longest_side;
};


/** Class for interpolation of 3D surface fields from external data.
 * Used for interpolation of boundary conditions between different meshes.
 * The source cell or point data are defined on a triangulated surface.
 */
class SurfaceInterpolator3D
{
private:
  /** Only 3D meshes are supported
   */
  constexpr static unsigned int dim = 3;

public:
  /** Field type
   */
  enum FieldType
  {
    CellField, ///< Cell field
    PointField ///< Point field
  };

  /** Read mesh and fields from \c vtk file
   */
  inline void
  read_vtk(const std::string &file_name);

  /** Read mesh and fields from \c vtu file
   */
  inline void
  read_vtu(const std::string &file_name);

  /** Write mesh and fields to \c vtu file
   */
  inline void
  write_vtu(const std::string &file_name) const;

  /** Interpolate field to the specified points
   */
  inline void
  interpolate(const FieldType &              field_type,
              const std::string &            field_name,
              const std::vector<Point<dim>> &target_points,
              const std::vector<bool> &      markers,
              Vector<double> &               target_values) const;

  /** Same as above, for target points in 2D
   */
  inline void
  interpolate(const FieldType &                  field_type,
              const std::string &                field_name,
              const std::vector<Point<dim - 1>> &target_points,
              const std::vector<bool> &          markers,
              Vector<double> &                   target_values) const;

  /** Convert between cell and point fields
   * If target_name is not specified it is set to source_name.
   */
  inline void
  convert(const FieldType &  source_type,
          const std::string &source_name,
          const FieldType &  target_type,
          const std::string &target_name = "");

private:
  /** Points \c (x,y,z)
   */
  std::vector<Point<dim>> points;

  /** Connectivity matrix \c (v0,v1,v2) - only triangles
   */
  std::vector<std::array<unsigned long, 3>> triangles;

  /** Fields defined on cells
   */
  std::map<std::string, std::vector<double>> cell_fields;

  /** Fields defined on points
   */
  std::map<std::string, std::vector<double>> point_fields;

  /** Vector fields defined on cells
   */
  std::map<std::string, std::vector<Point<dim>>> cell_vector_fields;

  /** Precalculated triangle areas, normals, centers etc.
   */
  std::vector<Triangle<dim>> triangle_cache;


  /** Get field
   */
  inline const std::vector<double> &
  field(const FieldType &field_type, const std::string &field_name) const;

  /** Get field
   */
  inline std::vector<double> &
  field(const FieldType &field_type, const std::string &field_name);

  /** Get vector field
   */
  inline const std::vector<Point<dim>> &
  vector_field(const FieldType &  field_type,
               const std::string &field_name) const;

  /** Convert cell field to point field
   */
  inline void
  cell_to_point(const std::string &source_name, const std::string &target_name);

  /** Convert point field to cell field
   */
  inline void
  point_to_cell(const std::string &source_name, const std::string &target_name);

  /** Clear all data
   */
  inline void
  clear();

  /** Print mesh and field information
   */
  inline void
  info() const;

  /** Precalculate auxiliary data (cell areas, centers, normals)
   */
  inline void
  preprocess();
};

/** Helper error message for failed interpolation
 */
template <int dim>
DeclException1(ExcInterpolationFailed,
               Point<dim>,
               << "Interpolation at point " << arg1 << " failed.");

/** Class for interpolation of 2D surface fields from external data.
 * Used for interpolation of boundary conditions between different meshes.
 * The source point data are defined on a polyline.
 */
class SurfaceInterpolator2D
{
private:
  /** Only 2D meshes are supported
   */
  constexpr static unsigned int dim = 2;

public:
  /** Read mesh and fields from plain text file
   */
  inline void
  read_txt(const std::string &file_name);

  /** Interpolate field to the specified points
   */
  inline void
  interpolate(const std::string &            field_name,
              const std::vector<Point<dim>> &target_points,
              const std::vector<bool> &      markers,
              Vector<double> &               target_values) const;

  /** Same as above, for target points in 3D
   */
  inline void
  interpolate(const std::string &                field_name,
              const std::vector<Point<dim + 1>> &target_points,
              const std::vector<bool> &          markers,
              Vector<double> &                   target_values) const;

private:
  /** Points \c (x,y)
   */
  std::vector<Point<dim>> points;

  /** Fields defined on points
   */
  std::map<std::string, std::vector<double>> fields;

  /** Get field
   */
  inline const std::vector<double> &
  field(const std::string &field_name) const;

  /** Clear all data
   */
  inline void
  clear();

  /** Print mesh and field information
   */
  inline void
  info() const;
};


// IMPLEMENTATION

double
sqr(const double x)
{
  return x * x;
}

std::vector<double>
split_string(const std::string &s, const char delimiter)
{
  const auto s_split = Utilities::split_string_list(s, delimiter);
  return Utilities::string_to_double(s_split);
}

void
initialize_function(std::unique_ptr<Function<1>> &f,
                    const std::string &           expression,
                    const std::string &           vars)
{
  const std::string ext =
    expression.size() <= 4 ? "" : expression.substr(expression.size() - 4);

  if (ext == ".txt" || ext == ".dat" || ext == ".tsv")
    {
      std::ifstream infile(expression);
      AssertThrow(infile, ExcFileNotOpen(expression.c_str()));

      std::vector<double> points, data;

      std::string line;

      while (std::getline(infile, line))
        {
          std::stringstream ss(line);

          double x, y;
          ss >> x >> y;

          points.push_back(x);
          data.push_back(y);
        }

      using F = Functions::InterpolatedTensorProductGridData<1>;

      const unsigned int n = points.size();

#ifdef DEAL_II_WITH_CXX14
      f = std::make_unique<F>(std::array<std::vector<double>, 1>{points},
                              Table<1, double>{n, data.begin()});
#else
      f = std::unique_ptr<F>(new F(std::array<std::vector<double>, 1>{points},
                                   Table<1, double>{n, data.begin()}));
#endif
    }
  else
    {
      using F = FunctionParser<1>;

#ifdef DEAL_II_WITH_CXX14
      f = std::make_unique<F>();
#else
      f = std::unique_ptr<F>(new F());
#endif

      auto *fp = dynamic_cast<F *>(f.get());
      fp->initialize(vars, expression, F::ConstMap());
    }
}

template <class T>
std::pair<T, T>
minmax(const Vector<T> &x)
{
  const auto mm = std::minmax_element(x.begin(), x.end());
  return std::make_pair(*mm.first, *mm.second);
}

template <class T>
std::pair<T, T>
minmax(const BlockVector<T> &x)
{
  std::vector<T> data;

  for (unsigned int i = 0; i < x.n_blocks(); ++i)
    {
      const auto mm = std::minmax_element(x.block(i).begin(), x.block(i).end());
      data.push_back(*mm.first);
      data.push_back(*mm.second);
    }
  const auto mm = std::minmax_element(data.begin(), data.end());
  return std::make_pair(*mm.first, *mm.second);
}

double
dx_analytical(const double a, const double b, const double dt)
{
  if (b == 0)
    return a * dt; // a*dt + b*sqr(dt)/2

  return a / b * (std::exp(b * dt) - 1);
}

std::string
format_time(const double x)
{
  return "(" + std::to_string(x) + " s)";
}

std::string
format_time(const Timer &timer)
{
  return format_time(timer.wall_time());
}

std::vector<std::string>
coordinate_names(const unsigned int dim)
{
  if (dim == 1)
    return {"x"};
  if (dim == 2)
    return {"r", "z"};
  if (dim == 3)
    return {"x", "y", "z"};

  AssertThrow(false, ExcNotImplemented());
  return {};
}

template <typename T>
void
write_data(const T &data, const std::string &file_name)
{
  try
    {
      std::cout << "Saving to '" << file_name << "'\n";
      std::ofstream f(file_name);
      data.block_write(f);
    }
  catch (std::exception &e)
    {
      std::cout << e.what() << "\n";
    }
}

template <typename T>
void
read_data(T &data, const std::string &file_name)
{
  try
    {
      std::cout << "Reading from '" << file_name << "'\n";
      std::ifstream f(file_name);
      data.block_read(f);
    }
  catch (std::exception &e)
    {
      std::cout << e.what() << "\n";
    }
}

template <int dim>
void
output_boundary_field_at_quadrature_points(const DoFHandler<dim> &dh,
                                           FEFaceValues<dim> &   fe_face_values,
                                           const Vector<double> &field,
                                           const unsigned int    boundary_id,
                                           const std::string &   file_name)
{
  try
    {
      Timer timer;
      std::cout << "Saving to '" << file_name;
      std::ofstream output(file_name);

      const auto dims = coordinate_names(dim);
      for (const auto &d : dims)
        output << d << "[m]\t";

      output << "f\n";


      const Quadrature<dim - 1> &face_quadrature =
        fe_face_values.get_quadrature();
      const unsigned int n_face_q_points = face_quadrature.size();

      std::vector<double> field_face_q(n_face_q_points);

      typename DoFHandler<dim>::active_cell_iterator cell = dh.begin_active(),
                                                     endc = dh.end();
      for (; cell != endc; ++cell)
        {
          for (unsigned int face_number = 0;
               face_number < GeometryInfo<dim>::faces_per_cell;
               ++face_number)
            {
              if (!cell->face(face_number)->at_boundary())
                continue;

              if (cell->face(face_number)->boundary_id() != boundary_id)
                continue;

              fe_face_values.reinit(cell, face_number);
              fe_face_values.get_function_values(field, field_face_q);

              for (unsigned int q = 0; q < n_face_q_points; ++q)
                {
                  const Point<dim> &p = fe_face_values.quadrature_point(q);

                  for (unsigned int d = 0; d < dim; ++d)
                    output << p[d] << '\t';

                  output << field_face_q[q] << '\n';
                }
            }
        }
      std::cout << " " << format_time(timer) << "\n";
    }
  catch (std::exception &e)
    {
      std::cout << e.what() << "\n";
    }
}

template <int dim>
Point<dim>
closest_segment_point(const Point<dim> &p,
                      const Point<dim> &segment_p0,
                      const Point<dim> &segment_p1)
{
  const auto d = segment_p1 - segment_p0;
  double     t = d * (p - segment_p0) / d.norm_square();
  // clamp to [0,1]
  t = std::max(0.0, std::min(1.0, t));

  return segment_p0 + t * d;
}

template <int dim>
std::array<double, 2>
barycentric_coordinates(const Point<dim> &p,
                        const Point<dim> &segment_p0,
                        const Point<dim> &segment_p1)
{
  const auto   d = segment_p1 - segment_p0;
  const double t = d * (p - segment_p0) / d.norm_square();

  // t=0 corresponds to the first point segment_p0
  return std::array<double, 2>({1.0 - t, t});
}

// Triangle

template <int dim>
void
Triangle<dim>::reinit(const Point<dim> &p0,
                      const Point<dim> &p1,
                      const Point<dim> &p2)
{
  m_points[0] = p0;
  m_points[1] = p1;
  m_points[2] = p2;

  calculate_normal_and_area();

  m_center = (p0 + p1 + p2) / 3;

  m_longest_side =
    std::max(std::max((p1 - p0).norm(), (p2 - p0).norm()), (p1 - p2).norm());
}

template <int dim>
Point<dim>
Triangle<dim>::center() const
{
  return m_center;
}

template <int dim>
Point<dim>
Triangle<dim>::normal() const
{
  return m_normal;
}

template <int dim>
double
Triangle<dim>::area() const
{
  return m_area;
}

template <int dim>
double
Triangle<dim>::longest_side() const
{
  return m_longest_side;
}

template <int dim>
Point<dim>
Triangle<dim>::closest_triangle_point(const Point<dim> &p) const
{
  const Point<dim> p_proj = project_to_triangle_plane(p);

  const auto t3 = barycentric_coordinates(p_proj);

  bool inside = true;

  for (const auto &t : t3)
    inside = inside && (t >= 0) && (t <= 1);

  if (inside)
    {
      return p_proj;
    }
  else
    {
      Point<dim> p_closest;
      double     d2_min = -1;

      for (unsigned int i = 0; i < 3; ++i)
        {
          const Point<dim> p_edge =
            closest_segment_point(p, m_points[i], m_points[(i + 1) % 3]);
          const double d2 = (p - p_edge).norm_square();

          if (d2 < d2_min || d2_min < 0)
            {
              d2_min    = d2;
              p_closest = p_edge;
            }
        }

      return p_closest;
    }
}

template <int dim>
void
Triangle<dim>::calculate_normal_and_area()
{
  m_normal = Point<dim>(
    cross_product_3d(m_points[1] - m_points[0], m_points[2] - m_points[0]));

  m_area = 0.5 * m_normal.norm();

  // normalize
  if (m_area > 0)
    m_normal /= (2 * m_area);
}

template <int dim>
double
Triangle<dim>::signed_area(const Point<dim> &p0,
                           const Point<dim> &p1,
                           const Point<dim> &p2) const
{
  return 0.5 * (m_normal * cross_product_3d(p1 - p0, p2 - p0));
}

template <int dim>
Point<dim>
Triangle<dim>::project_to_triangle_plane(const Point<dim> &p) const
{
  return Point<dim>(p - m_normal * (m_normal * (p - m_points[0])));
}

template <int dim>
std::array<double, 3>
Triangle<dim>::barycentric_coordinates(const Point<dim> &p) const
{
  return std::array<double, 3>(
    {signed_area(p, m_points[1], m_points[2]) / m_area,
     signed_area(m_points[0], p, m_points[2]) / m_area,
     signed_area(m_points[0], m_points[1], p) / m_area});
}

// SurfaceInterpolator3D

void
SurfaceInterpolator3D::read_vtk(const std::string &file_name)
{
  Timer timer;

  clear();

  std::ifstream file(file_name);

  if (!file.is_open())
    {
      std::cout << "Could not open '" << file_name << "'\n";
      return;
    }
  else
    std::cout << "Reading '" << file_name << "'";


  std::string s, data_type, data_name;

  // First line should be "# vtk DataFile Version 4.2"
  while (file >> s)
    {
      if (s == "POINTS")
        {
          unsigned int n;
          file >> n >> s /*data type*/;
          points.resize(n);

          for (unsigned int i = 0; i < n; ++i)
            {
              file >> points[i][0] >> points[i][1] >> points[i][2];
            }
        }
      else if (s == "CELLS")
        {
          unsigned int n;
          file >> n >> s /*size*/;
          triangles.resize(n);

          for (unsigned int i = 0; i < n; ++i)
            {
              file >> s;
              if (s != "3")
                throw std::runtime_error("Triangle expected, numPoints=" + s +
                                         "found");
              file >> triangles[i][0] >> triangles[i][1] >> triangles[i][2];
            }
        }
      else
        {
          if (s == "CELL_DATA" || s == "POINT_DATA")
            data_type = s;

          if (s == "SCALARS")
            {
              file >> data_name >> s /*data type*/;
              file >> s >> s; // LOOKUP_TABLE

              unsigned int N =
                data_type == "CELL_DATA" ? triangles.size() : points.size();

              std::vector<double> &f = data_type == "CELL_DATA" ?
                                         cell_fields[data_name] :
                                         point_fields[data_name];
              f.resize(N);

              for (unsigned int i = 0; i < N; ++i)
                file >> f[i];
            }

          if (s == "FIELD")
            {
              unsigned int n_fields;
              file >> s /*FieldData*/ >> n_fields;

              for (unsigned int k = 0; k < n_fields; ++k)
                {
                  file >> data_name >> s >> s >> s;

                  unsigned int N =
                    data_type == "CELL_DATA" ? triangles.size() : points.size();

                  std::vector<double> &f = data_type == "CELL_DATA" ?
                                             cell_fields[data_name] :
                                             point_fields[data_name];
                  f.resize(N);

                  for (unsigned int i = 0; i < N; ++i)
                    file >> f[i];
                }
            }
        }
    }

  std::cout << " " << format_time(timer) << "\n";

  info();
  preprocess();
}

void
SurfaceInterpolator3D::read_vtu(const std::string &file_name)
{
  Timer timer;

  clear();

  std::ifstream file(file_name);

  if (!file.is_open())
    {
      std::cout << "Could not open '" << file_name << "'\n";
      return;
    }
  else
    std::cout << "Reading '" << file_name << "'";

  std::string s, data_type, data_name;

  bool data_start = false;

  while (file >> s)
    {
      // get number of points and cells
      if (Utilities::match_at_string_start(s, "NumberOf"))
        {
          std::vector<std::string> l = Utilities::split_string_list(s, '"');

          int n = Utilities::string_to_int(l[1]);

          if (l[0] == "NumberOfPoints=")
            points.resize(n);
          else if (l[0] == "NumberOfCells=")
            triangles.resize(n);

          continue;
        }

      // detect cell and point data (also points and cells)
      if (Utilities::match_at_string_start(s, "<Cell") ||
          Utilities::match_at_string_start(s, "<Point"))
        data_type = s;

      // get name
      if (Utilities::match_at_string_start(s, "Name="))
        data_name = Utilities::split_string_list(s, '"')[1];

      // prepare for reading data
      if (s == "<DataArray")
        data_start = true;

      // <DataArray ...> has been reached, now read the data
      if (data_start && s.back() == '>')
        {
          std::vector<std::string> data;

          while (file >> s)
            {
              // end of data array reached, process it
              if (s == "</DataArray>")
                {
                  const unsigned int n = data.size();

                  if (data_type == "<CellData>") // cell field
                    {
                      if (n != triangles.size())
                        throw std::runtime_error(
                          data_type + " " + std::to_string(triangles.size()) +
                          " " + data_name + " " + std::to_string(n));

                      auto &field = cell_fields[data_name];
                      field.resize(n);

                      for (unsigned int i = 0; i < n; ++i)
                        field[i] = std::stod(data[i]);
                    }
                  else if (data_type == "<PointData>") // point field
                    {
                      if (n != points.size())
                        throw std::runtime_error(
                          data_type + " " + std::to_string(points.size()) +
                          " " + data_name + " " + std::to_string(n));

                      auto &field = point_fields[data_name];
                      field.resize(n);

                      for (unsigned int i = 0; i < n; ++i)
                        field[i] = std::stod(data[i]);
                    }
                  else if (data_type == "<Points>") // point coordinates
                    {
                      if (n != 3 * points.size())
                        throw std::runtime_error(
                          data_type + " " + std::to_string(points.size()) +
                          " " + data_name + " " + std::to_string(n));

                      for (unsigned int i = 0; i < n / 3; ++i)
                        {
                          unsigned int k = 3 * i;

                          points[i] = Point<dim>(std::stod(data[k]),
                                                 std::stod(data[k + 1]),
                                                 std::stod(data[k + 2]));
                        }
                    }
                  else if (data_type == "<Cells>" &&
                           data_name == "connectivity") // only connectivity
                    {
                      if (n != 3 * triangles.size())
                        throw std::runtime_error(
                          data_type + " " + std::to_string(points.size()) +
                          " " + data_name + " " + std::to_string(n));

                      for (unsigned int i = 0; i < n / 3; ++i)
                        {
                          unsigned int k = 3 * i;

                          triangles[i] = {std::stoul(data[k]),
                                          std::stoul(data[k + 1]),
                                          std::stoul(data[k + 2])};
                        }
                    }

                  data_start = false;
                  data_name.clear();
                  break;
                }
              else // append
                data.push_back(s);
            }
        }
    }

  std::cout << " " << format_time(timer) << "\n";

  info();
  preprocess();
}

void
SurfaceInterpolator3D::write_vtu(const std::string &file_name) const
{
  Timer timer;

  std::cout << "Saving to '" << file_name << "'";

  std::ofstream f_out(file_name);

  const unsigned int n_points    = points.size();
  const unsigned int n_triangles = triangles.size();

  f_out
    << "<?xml version=\"1.0\"?>\n"
       "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">\n"
       "<UnstructuredGrid>\n"
       "<Piece NumberOfPoints=\""
    << n_points << "\" NumberOfCells=\"" << n_triangles << "\">\n";


  f_out << "<CellData>\n";
  for (const auto &it : cell_fields)
    {
      f_out << "<DataArray type=\"Float64\" Name=\"" << it.first
            << "\" format=\"ascii\">\n";
      for (const auto &x : it.second)
        f_out << std::scientific << std::setprecision(14) << x << " ";
      f_out << "\n</DataArray>\n";
    }
  for (const auto &it : cell_vector_fields)
    {
      f_out << "<DataArray type=\"Float64\" Name=\"" << it.first
            << "\" NumberOfComponents=\"3\" format=\"ascii\">\n";
      for (const auto &x : it.second)
        f_out << std::scientific << std::setprecision(14) << x << "\n";
      f_out << "</DataArray>\n";
    }
  f_out << "</CellData>\n";


  f_out << "<PointData>\n";
  for (const auto &it : point_fields)
    {
      f_out << "<DataArray type=\"Float64\" Name=\"" << it.first
            << "\" format=\"ascii\">\n";
      for (const auto &x : it.second)
        f_out << std::scientific << std::setprecision(14) << x << " ";
      f_out << "\n</DataArray>\n";
    }
  f_out << "</PointData>\n";


  f_out << "<Points>\n";
  f_out
    << "<DataArray type=\"Float32\" NumberOfComponents=\"3\" format=\"ascii\">\n";
  for (const auto &p : points)
    f_out << std::scientific << std::setprecision(14) << p << "\n";
  f_out << "</DataArray>\n";
  f_out << "</Points>\n";


  f_out << "<Cells>\n";

  f_out
    << "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n";
  for (const auto &t : triangles)
    {
      for (const auto &v : t)
        f_out << v << " ";
      f_out << "\n";
    }
  f_out << "</DataArray>\n";

  f_out << "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n";
  for (unsigned int i = 0; i < n_triangles; ++i)
    f_out << 3 * (i + 1) << " ";
  f_out << "\n</DataArray>\n";

  f_out << "<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n";
  for (unsigned int i = 0; i < n_triangles; ++i)
    f_out << "5 "; // VTK_TRIANGLE
  f_out << "\n</DataArray>\n";

  f_out << "</Cells>\n";


  f_out << "</Piece>\n"
           "</UnstructuredGrid>\n"
           "</VTKFile>\n";

  std::cout << " " << format_time(timer) << "\n";
}

void
SurfaceInterpolator3D::interpolate(const FieldType &              field_type,
                                   const std::string &            field_name,
                                   const std::vector<Point<dim>> &target_points,
                                   const std::vector<bool> &      markers,
                                   Vector<double> &target_values) const
{
  AssertThrow(field_type == CellField || field_type == PointField,
              ExcNotImplemented());

  Timer timer;

  std::cout << "Interpolating field '" << field_name << "'";

  const std::vector<double> &source_field = field(field_type, field_name);

  const unsigned int n_triangles = triangles.size();
  const unsigned int n_values    = target_points.size();
  target_values.reinit(n_values);

  Triangle<dim> triangle;

  for (unsigned int i = 0; i < n_values; ++i)
    {
      if (!markers[i])
        continue;

      Point<dim>   p_found;
      unsigned int j_found = 0;
      double       d2_min  = -1;
      for (unsigned int j = 0; j < n_triangles; ++j)
        {
          const Triangle<dim> &triangle = triangle_cache[j];

          if ((target_points[i] - triangle.center()).norm() >
              3 * triangle.longest_side())
            continue;

          Point<dim> p_trial =
            triangle.closest_triangle_point(target_points[i]);

          double d2 = (p_trial - target_points[i]).norm_square();
          if (d2 < d2_min || d2_min < 0)
            {
              d2_min  = d2;
              p_found = p_trial;
              j_found = j;
            }
        }

      if (d2_min < 0)
        {
#ifdef DEBUG
          std::cout << "Warning: point " << target_points[i]
                    << " not found, continuing with full search\n";
#endif
          // same as above, now checking each triangle
          for (unsigned int j = 0; j < n_triangles; ++j)
            {
              const Triangle<dim> &triangle = triangle_cache[j];

              Point<dim> p_trial =
                triangle.closest_triangle_point(target_points[i]);

              double d2 = (p_trial - target_points[i]).norm_square();
              if (d2 < d2_min || d2_min < 0)
                {
                  d2_min  = d2;
                  p_found = p_trial;
                  j_found = j;
                }
            }
        }
      AssertThrow(d2_min >= 0, ExcInterpolationFailed<dim>(target_points[i]));

      switch (field_type)
        {
          case CellField:
            target_values[i] = source_field[j_found];
            break;

          case PointField:
            const Triangle<dim> &triangle = triangle_cache[j_found];
            const auto           t3 = triangle.barycentric_coordinates(p_found);
            const auto &         v  = triangles[j_found];
            for (unsigned int k = 0; k < 3; ++k)
              target_values[i] += t3[k] * source_field[v[k]];
            break;
        }
    }

  std::cout << " " << format_time(timer) << "\n";
}

void
SurfaceInterpolator3D::interpolate(
  const FieldType &                  field_type,
  const std::string &                field_name,
  const std::vector<Point<dim - 1>> &target_points,
  const std::vector<bool> &          markers,
  Vector<double> &                   target_values) const
{
  const unsigned int n_values = target_points.size();

  std::vector<Point<dim>> points_3d(n_values);

  // convert from 2D cylindrical coordinates (r,z) to 3D (x,y,z)
  for (unsigned int i = 0; i < n_values; ++i)
    {
      points_3d[i][0] = target_points[i][0];
      points_3d[i][1] = 0;
      points_3d[i][2] = target_points[i][1];
    }

  interpolate(field_type, field_name, points_3d, markers, target_values);
}

void
SurfaceInterpolator3D::convert(const FieldType &  source_type,
                               const std::string &source_name,
                               const FieldType &  target_type,
                               const std::string &target_name)
{
  const std::string target_name_updated =
    target_name.empty() ? source_name : target_name;

  if (source_type == CellField && target_type == PointField)
    {
      cell_to_point(source_name, target_name_updated);
    }
  else if (source_type == PointField && target_type == CellField)
    {
      point_to_cell(source_name, target_name_updated);
    }
  else
    {
      throw std::runtime_error(
        "Unsupported combination of source and target field types.");
    }
}

const std::vector<double> &
SurfaceInterpolator3D::field(const FieldType &  field_type,
                             const std::string &field_name) const
{
  const auto &fields = field_type == CellField ? cell_fields : point_fields;
  const auto &it     = fields.find(field_name);

  if (it == fields.end())
    throw std::runtime_error("Field '" + field_name + "' does not exist.");

  return it->second;
}

std::vector<double> &
SurfaceInterpolator3D::field(const FieldType &  field_type,
                             const std::string &field_name)
{
  return field_type == CellField ? cell_fields[field_name] :
                                   point_fields[field_name];
}

const std::vector<Point<SurfaceInterpolator3D::dim>> &
SurfaceInterpolator3D::vector_field(const FieldType &  field_type,
                                    const std::string &field_name) const
{
  AssertThrow(field_type == CellField, ExcNotImplemented());

  const auto &fields = cell_vector_fields;
  const auto &it     = fields.find(field_name);

  if (it == fields.end())
    throw std::runtime_error("Field '" + field_name + "' does not exist.");

  return it->second;
}

void
SurfaceInterpolator3D::cell_to_point(const std::string &source_name,
                                     const std::string &target_name)
{
  Timer timer;

  std::cout << "Convering field '" << source_name << "' from cell to point";

  const unsigned int n_points    = points.size();
  const unsigned int n_triangles = triangles.size();

  const std::vector<double> &source_field = field(CellField, source_name);

  std::vector<double> &target_field = field(PointField, target_name);
  target_field.resize(n_points);
  std::fill(target_field.begin(), target_field.end(), 0);

  std::vector<int> count(n_points, 0);

  for (unsigned int i = 0; i < n_triangles; ++i)
    {
      const auto &v = triangles[i];
      for (const auto &id : v)
        {
          target_field[id] += source_field[i];
          count[id]++;
        }
    }

  for (unsigned int i = 0; i < n_points; ++i)
    {
      if (count[i] > 0)
        target_field[i] /= count[i];
    }

  std::cout << " " << format_time(timer) << "\n";
}

void
SurfaceInterpolator3D::point_to_cell(const std::string &source_name,
                                     const std::string &target_name)
{
  Timer timer;

  std::cout << "Convering field '" << source_name << "' from point to cell";

  const unsigned int n_triangles = triangles.size();

  const std::vector<double> &source_field = field(PointField, source_name);

  std::vector<double> &target_field = field(CellField, target_name);
  target_field.resize(n_triangles);
  std::fill(target_field.begin(), target_field.end(), 0);

  std::vector<int> count(n_triangles, 0);

  for (unsigned int i = 0; i < n_triangles; ++i)
    {
      const auto &v = triangles[i];
      for (const auto &id : v)
        {
          target_field[i] += source_field[id];
          count[i]++;
        }
    }

  for (unsigned int i = 0; i < n_triangles; ++i)
    {
      if (count[i] > 0)
        target_field[i] /= count[i];
    }

  std::cout << " " << format_time(timer) << "\n";
}

void
SurfaceInterpolator3D::clear()
{
  points.clear();
  triangles.clear();
  cell_fields.clear();
  point_fields.clear();
  cell_vector_fields.clear();
  triangle_cache.clear();
}

void
SurfaceInterpolator3D::info() const
{
  std::cout << "n_points:" << points.size() << " "
            << "n_triangles:" << triangles.size() << "\n";

  for (const auto &it : cell_fields)
    std::cout << "CellData " << it.first << " " << it.second.size() << "\n";

  for (const auto &it : point_fields)
    std::cout << "PointData " << it.first << " " << it.second.size() << "\n";
}

void
SurfaceInterpolator3D::preprocess()
{
  Timer timer;

  std::cout << "Preprocessing data";

  const unsigned int n_triangles = triangles.size();

  auto &area         = cell_fields["area"];
  auto &longest_side = cell_fields["longest_side"];
  auto &center       = cell_vector_fields["center"];
  auto &normal       = cell_vector_fields["normal"];

  area.resize(n_triangles);
  longest_side.resize(n_triangles);
  center.resize(n_triangles);
  normal.resize(n_triangles);
  triangle_cache.resize(n_triangles);

  for (unsigned int i = 0; i < n_triangles; ++i)
    {
      const auto &   v        = triangles[i];
      Triangle<dim> &triangle = triangle_cache[i];
      triangle.reinit(points[v[0]], points[v[1]], points[v[2]]);

      // for output, should be moved to debug mode
      center[i]       = triangle.center();
      normal[i]       = triangle.normal();
      area[i]         = triangle.area();
      longest_side[i] = triangle.longest_side();
    }

  std::cout << " " << format_time(timer) << "\n";
}

// SurfaceInterpolator2D

void
SurfaceInterpolator2D::read_txt(const std::string &file_name)
{
  Timer timer;

  clear();

  std::ifstream file(file_name);

  if (!file.is_open())
    {
      std::cout << "Could not open '" << file_name << "'\n";
      return;
    }
  else
    std::cout << "Reading '" << file_name << "'";


  std::vector<std::string>         field_names;
  std::vector<std::vector<double>> field_values;

  std::string line;
  while (std::getline(file, line))
    {
      if (line.empty())
        continue;

      std::stringstream ss(line);

      if (field_names.empty())
        {
          std::string name;
          ss >> name >> name; // First 2 columns are x and y;
          while (ss >> name)  // then - field names
            field_names.push_back(name);

          field_values.resize(field_names.size());
        }
      else
        {
          Point<dim> p;
          ss >> p[0] >> p[1];

          points.push_back(p);

          double val;
          for (unsigned int i = 0; i < field_names.size(); ++i)
            {
              ss >> val;
              field_values[i].push_back(val);
            }
        }
    }

  for (unsigned int i = 0; i < field_names.size(); ++i)
    fields[field_names[i]] = field_values[i];

  std::cout << " " << format_time(timer) << "\n";

  info();
}

void
SurfaceInterpolator2D::interpolate(const std::string &            field_name,
                                   const std::vector<Point<dim>> &target_points,
                                   const std::vector<bool> &      markers,
                                   Vector<double> &target_values) const
{
  Timer timer;

  std::cout << "Interpolating field '" << field_name << "'";

  const std::vector<double> &source_field = field(field_name);

  const unsigned int n_points = points.size();
  const unsigned int n_values = target_points.size();
  target_values.reinit(n_values);

  for (unsigned int i = 0; i < n_values; ++i)
    {
      if (!markers[i])
        continue;

      Point<dim>   p_found;
      unsigned int j_found = 0;
      double       d2_min  = -1;

      // number of segments is n_points-1
      for (unsigned int j = 0; j + 1 < n_points; ++j)
        {
          Point<dim> p_trial =
            closest_segment_point(target_points[i], points[j], points[j + 1]);

          double d2 = (p_trial - target_points[i]).norm();
          if (d2 < d2_min || d2_min < 0)
            {
              d2_min  = d2;
              p_found = p_trial;
              j_found = j;
            }
        }

      const auto t2 =
        barycentric_coordinates(p_found, points[j_found], points[j_found + 1]);
      for (unsigned int k = 0; k < 2; ++k)
        target_values[i] += t2[k] * source_field[j_found + k];
    }

  std::cout << " " << format_time(timer) << "\n";
}

void
SurfaceInterpolator2D::interpolate(
  const std::string &                field_name,
  const std::vector<Point<dim + 1>> &target_points,
  const std::vector<bool> &          markers,
  Vector<double> &                   target_values) const
{
  const unsigned int n_values = target_points.size();

  std::vector<Point<dim>> points_2d(n_values);

  // convert from 3D (x,y,z) to 2D cylindrical coordinates (r,z)
  for (unsigned int i = 0; i < n_values; ++i)
    {
      points_2d[i][0] = std::hypot(target_points[i][0], target_points[i][1]);
      points_2d[i][1] = target_points[i][2];
    }

  interpolate(field_name, points_2d, markers, target_values);
}

const std::vector<double> &
SurfaceInterpolator2D::field(const std::string &field_name) const
{
  const auto &it = fields.find(field_name);

  if (it == fields.end())
    throw std::runtime_error("Field '" + field_name + "' does not exist.");

  return it->second;
}

void
SurfaceInterpolator2D::clear()
{
  points.clear();
  fields.clear();
}

void
SurfaceInterpolator2D::info() const
{
  std::cout << "n_points:" << points.size() << "\n";

  for (const auto &it : fields)
    std::cout << "PointData " << it.first << " " << it.second.size() << "\n";
}

#endif