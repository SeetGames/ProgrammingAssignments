/*!*****************************************************************************
 \file polynomial.h
 \author Seetoh Wei Tung
 \par DP email: seetoh.w@digipen.edu
 \par Course:  Modern C++ Design Patterns
 \par Section: B
 \par Take Home Programming Quiz 5
 \date 22-10-2023
 \brief
    Declares interface to Polynomial class template that represents a polynomial.
    It takes a template type parameter specifying the type of coefficients and a
    non-template type parameter specifying the polynomial's degree.
*******************************************************************************/

#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <iostream> // std::ostream

namespace HLP3
{
    /*!*****************************************************************************
     * @brief A Polynomial class template. CoefficientType is the type of the
        coefficients of the polynomial. Degree is the degree of the polynomial. Note
        that the degree of a polynomial must be a non-negative integer. The degree of
        a polynomial is the highest exponent of the variable in the polynomial. For
        example, the degree of 1+x+x^2+x^3 is 3. The degree of 1+x+x^2+x^3+x^100 is
        100. The degree of 1 is 0. The degree of 0 is undefined.
        The coefficients of a polynomial are stored in an array of type CoefficientType.
     * 
     * @tparam CoefficientType
     * @tparam Degree
    *******************************************************************************/
    template <typename CoefficientType, int Degree>
    class Polynomial
    {
    public:
        /*!*****************************************************************************
         * @brief Default constructor. Creates a zero polynomial
         * 
        *******************************************************************************/
        Polynomial();

        /*!*****************************************************************************
         * @brief Single argument conversion constructor. Creates a polynomial from
         another polynomial. Note that a polynomial of degree m cannot be constructed
         with another polynomial of degree n. However, the types of coefficients for
         polynomials could be different.
         example: HLP3::Polynomial<int,2> p2( pf_1 ); // convert
         * 
         * @param _copy 
        *******************************************************************************/
        template <typename CopyCoefficientType>
        Polynomial(Polynomial<CopyCoefficientType, Degree> const& _copy);

        /*!*****************************************************************************
         * @brief Copy assignment operator. Assigns a polynomial to another polynomial.
         Note that both polynomial must have the same degree. However, the types of
         coefficients for polynomials could be different.
         * 
         * @param _copy
         * @return Polynomial<CoefficientType, Degree>& Copy of the polynomial
        *******************************************************************************/
        template <typename RhsCoefficientType>
        Polynomial<CoefficientType, Degree> &operator=(Polynomial<RhsCoefficientType, Degree> const& _rhs);

        /*!*****************************************************************************
         * @brief Modifiable operator[]. Returns a reference to the coefficient of the
            term with the given exponent.
         * 
         * @param _exponent 
         * @return CoefficientType& 
        *******************************************************************************/
        CoefficientType &operator[](int _exponent);

        /*!*****************************************************************************
         * @brief Non-modifiable operator[]. Returns a reference to the coefficient of 
         the term with the given exponent.
         * 
         * @param _exponent 
         * @return CoefficientType const& 
        *******************************************************************************/
        CoefficientType const &operator[](int _exponent) const;

        /*!*****************************************************************************
         * @brief Given polynomials p(x) and q(x), return p(x)*q(x). Note that this
            function must only allow polynomials with similar coefficient types to be
            multiplied.
         * 
         * @param _rhs 
         * @return Polynomial<CoefficientType, Degree> 
        *******************************************************************************/
        template <int RhsDegree>
        Polynomial<CoefficientType, Degree + RhsDegree> operator*(Polynomial<CoefficientType, RhsDegree> const& _rhs) const;

        /*!*****************************************************************************
         * @brief Evaluage polynomial at x = a. Evaluate polynomial p(x) using 
         argument x and return the value.
         * 
         * @param _x 
         * @return CoefficientType 
        *******************************************************************************/
        CoefficientType operator()(CoefficientType const& _x) const;
    private:
        CoefficientType m_Coefficients[Degree + 1];
    };
     // Overload operator<<
    template <typename CoefficientType, int Degree>
    std::ostream &operator<<(std::ostream &, Polynomial<CoefficientType, Degree> const &);
}


#include "polynomial.tpp"

#endif
