/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package web;

import ejb.Book;
import ejb.BookFacadeLocal;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.annotation.Resource;
import javax.ejb.EJB;
import javax.jms.Connection;
import javax.jms.ConnectionFactory;
import javax.jms.JMSException;
import javax.jms.MessageProducer;
import javax.jms.ObjectMessage;
import javax.jms.Queue;
import javax.jms.Session;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

/**
 *
 * @author erwan
 */
@WebServlet(name = "BookCreator", urlPatterns = {"/BookCreator"})
public class BookCreator extends HttpServlet {

    @EJB
    private BookFacadeLocal bookFacade;

    @Resource(mappedName = "NewMessageFactory")
    private ConnectionFactory connectionFactory;
    @Resource(mappedName = "NewMessage")
    private Queue queue;

    /**
     * Processes requests for both HTTP <code>GET</code> and <code>POST</code>
     * methods.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    protected void processRequest(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        HttpSession session = request.getSession(true);
        try {
            if (session.getAttribute("user") != null) {
                this.getServletContext().getRequestDispatcher("/index.jsp").forward(request, response);
            }
            String title = request.getParameter("title");
            String author = request.getParameter("author");
            String date = request.getParameter("date");
            String price = request.getParameter("price");
            try {

                Connection connection = connectionFactory.createConnection();
                Session conSession = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
                MessageProducer messageProducer = conSession.createProducer(queue);

                ObjectMessage message = conSession.createObjectMessage();

                Book book = new Book(title, author, Integer.parseInt(date), Float.parseFloat(price));
                message.setStringProperty("type", "book");
                message.setObject(book);

                messageProducer.send(message);
                messageProducer.close();
                connection.close();
                this.getServletContext().getRequestDispatcher("/index.jsp").forward(request, response);

            } catch (JMSException ex) {
                Logger.getLogger(BookCreator.class.getName()).log(Level.SEVERE, null, ex);
            }

        } catch (Exception e) {
            this.getServletContext().getRequestDispatcher("/index.jsp").forward(request, response);
        }
    }

    // <editor-fold defaultstate="collapsed" desc="HttpServlet methods. Click on the + sign on the left to edit the code.">
    /**
     * Handles the HTTP <code>GET</code> method.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        processRequest(request, response);
    }

    /**
     * Handles the HTTP <code>POST</code> method.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        processRequest(request, response);
    }

    /**
     * Returns a short description of the servlet.
     *
     * @return a String containing servlet description
     */
    @Override
    public String getServletInfo() {
        return "Short description";
    }// </editor-fold>

}